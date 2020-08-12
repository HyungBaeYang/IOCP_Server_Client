#pragma once
// ������ ���ǵ��� ���� ����ϰ� �߰��� ������ �̷�����Ƿ�
// ���ǵ��� �����ϴ� �Ŵ����� �ʿ��ϴ�.

#include "stdafx.h"
#include "Session.h"


#define SESSION_CAPACITY (5000)
#define SessionManageMent SessionManager::GetInstance()

class SessionManager : public Singleton<SessionManager>
{
	list<Session*> sessionList_;
	int sessiontCount_;
	int maxConnCount_;

	mutex mutex_;
	int8_t sessionSeed_;

public:
	SessionManager(int maxConnCount = SESSION_CAPACITY);
	~SessionManager();

	int8_t createSessionID();
	bool addSession(Session *session);

	list<Session*>& GetSessionList();
	bool closeSession(Session *session);
	void forceCloseSession(Session *session);
	Session *session(int8_t id);
};