#pragma once

struct GLFWwindow;
class GLApp
{
public:
	struct Camera
	{
		float fCamX;
		float fCamY;
		float fCamZ;
		float fCamAng;
	};

public:
	GLApp();
	~GLApp();

private:
	GLFWwindow* m_pWindow;
	int m_nRefreshRate;
	float m_fDTime;
	Camera m_cam;

	static float fTransZ;
	static float fTransX;
	static float fTransY;
	static float fEyeAngleX;
	static float fEyeAngleZ;
	static int bRClick;
	static float fMousePosX;
	static float fMousePosY;
	static bool bTrack;

private:
	void Lighting();
	void Render();

protected:
	bool GetEyeTrack() { return bTrack; };
	float GetDifftime() { return m_fDTime; };
	Camera GetCame() { return m_cam; };
	void SetCam(Camera& cam) { m_cam = cam; };
	GLFWwindow* GetWindow() { return m_pWindow; };

public:
	int Run();
	int InitWindows(int nWidth, int nHeight);
	int RefreshRate() const { return m_nRefreshRate; }
	void RefreshRate(int val) { m_nRefreshRate = val; }

protected:
	static void error_callback(int error, const char* description);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void win_size(GLFWwindow * window, int nW, int nH);
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void CalcView(GLFWwindow * window);

protected:
	virtual void CheckInput();
	virtual void Draw();
	virtual void CalcActor();
	virtual int Init();
	virtual int Finish();
public:
	
};