#pragma once

#include <vector>

template <typename T>
class Animation {
public:
	Animation(bool loop=true)
		: m_FrameTime(1.f/60.f), m_CurFrame(0), m_CurTime(0.f), m_Running(false), m_Loop(loop)
	{

	}

	Animation(int fps, bool loop=true)
		: m_FrameTime(1.f/fps), m_CurFrame(0), m_CurTime(0.f), m_Running(false), m_Loop(loop)
	{

	}

	Animation(const std::vector<T> &frames, int fps, bool loop=true)
		: m_FrameTime(1.f/fps), m_CurFrame(0), m_CurTime(0.f), m_Running(false), m_Loop(loop)
	{
		m_Frames(frames);
	}

	int GetFPS() const {
		return int(1.f/m_FrameTime);
	}

	void SetFPS(int fps) {
		m_FrameTime = 1.f/fps;
	}

	bool GetLoop() const {
		return m_Loop;
	}

	void SetLoop(bool b) {
		m_Loop = b;
	}
	
	void InsertFrame(const T frame) {
		m_Frames.push_back(frame);
	}

	T &GetFrame(int idx) const {
		return m_Frames[idx];
	}

	void SetCurrentFrame(int idx) {
		m_CurFrame = idx;
	}

	T GetCurrentFrame() const {
		return m_Frames[m_CurFrame];
	}

	void Tick(float dt) {
		if (m_Running) {
			m_CurTime += dt;
			if (m_CurTime >= m_FrameTime) {
				m_CurFrame++;
				m_CurTime -= m_FrameTime;

				if (m_CurFrame >= m_Frames.size()) {
					m_CurFrame = 0;

					if (!m_Loop)
						Stop();
				}
			}
		}
	}

	void Play() {
		m_Running = true;
	}

	void Stop() {
		m_Running = false;
		m_CurFrame = 0;
		m_CurTime = 0.f;
	}

	void Pause() {
		m_Running = false;
	}

	void Toggle() {
		if (m_Running)
			Stop();
		else
			Play();
	}

private:
	std::vector<T>	m_Frames;
	bool			m_Running;
	bool			m_Loop;

	float			m_FrameTime;
	float			m_CurTime;
	unsigned int	m_CurFrame;
};