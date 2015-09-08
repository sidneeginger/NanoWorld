#pragma once

struct GLFWwindow;
class GLApp
{
public:
	GLApp();
	~GLApp();
	int InitWindows(int nWidth, int nHeight);

	int RefreshRate() const { return m_nRefreshRate; }
	void RefreshRate(int val) { m_nRefreshRate = val; }

	static GLApp& Instance()
	{
		static GLApp instance;
		return instance;
	}

private:
	GLFWwindow* m_pWindow;
	int m_nRefreshRate;
	float m_fDTime;
	float fTransZ = -100.0f;
	float fTransX = 0.0f;
	float fTransY = 0.0f;
	float fEyeAngleX = -60.0f;
	float fEyeAngleZ = 0.0f;
	int bRClick = 0;
	float fMousePosX = 0.0f;
	float fMousePosY = 0.0f;
	bool bTrack = false;

protected:

	bool GetEyeTrack() { return bTrack; };
	float GetDifftime() { return m_fDTime; };

public:
	int Run();

protected:
	static void error_callback(int error, const char* description);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void win_size(GLFWwindow * window, int nW, int nH);
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void CalcView(GLFWwindow * window);

protected:
	void Handle_Error(int error, const char* description);
	void Handle_Keys(int key, int scancode, int action, int mods);

protected:
	virtual void Render() ;
	virtual void CheckInput();
public:
	void Lighting();
};

#define sGLApp GLApp::Instance()