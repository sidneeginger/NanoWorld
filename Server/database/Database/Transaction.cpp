#include "stdafx.h"

#include "DatabaseEnv.h"
#include "Transaction.h"
#include <mysqld_error.h>

std::mutex TransactionTask::_deadlockLock;

//- Append a raw ad-hoc query to the transaction
void Transaction::Append(const char* sql)
{
    SQLElementData data;
    data.type = SQL_ELEMENT_RAW;
    data.element.query = _strdup(sql);
    m_queries.push_back(data);
}

//- Append a prepared statement to the transaction
void Transaction::Append(PreparedStatement* stmt)
{
    SQLElementData data;
    data.type = SQL_ELEMENT_PREPARED;
    data.element.stmt = stmt;
    m_queries.push_back(data);
}

void Transaction::Cleanup()
{
    // This might be called by explicit calls to Cleanup or by the auto-destructor
    if (_cleanedUp)
        return;

    while (!m_queries.empty())
    {
        SQLElementData const &data = m_queries.front();
        switch (data.type)
        {
            case SQL_ELEMENT_PREPARED:
                delete data.element.stmt;
            break;
            case SQL_ELEMENT_RAW:
                free((void*)(data.element.query));
            break;
        }

        m_queries.pop_front();
    }

    _cleanedUp = true;
}

bool TransactionTask::Execute()
{
    int errorCode = m_conn->ExecuteTransaction(m_trans);
    if (!errorCode)
        return true;

    if (errorCode == ER_LOCK_DEADLOCK)
    {
        // Make sure only 1 async thread retries a transaction so they don't keep dead-locking each other
        std::lock_guard<std::mutex> lock(_deadlockLock);
        uint8 loopBreaker = 5;  // Handle MySQL Errno 1213 without extending deadlock to the core itself
        for (uint8 i = 0; i < loopBreaker; ++i)
            if (!m_conn->ExecuteTransaction(m_trans))
                return true;
    }

    // Clean up now.
    m_trans->Cleanup();

    return false;
}
