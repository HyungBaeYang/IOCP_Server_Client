#include "SessionManager.h"

SessionManager::SessionManager(int maxConnCount)
{
	sessionSeed_ = 1;
	maxConnCount_ = maxConnCount;
}

SessionManager::~SessionManager()
{
	vector<Session*> vec_removeSession;
	vec_removeSession.resize(sessionList_.size());
	copy(sessionList_.begin(), sessionList_.end(), vec_removeSession.begin());
	for (auto session : vec_removeSession) {
		session->onClose();

	}
	sessionList_.clear();
}

int8_t SessionManager::createSessionID()
{
	return sessionSeed_++;
}

bool SessionManager::addSession(Session * session)
{
	// �����ϴ� ���� ó�� �� Lock�� �ɾ��ش�.
	// golang ������ defer unlock�ϸ� �� ���ѵ�... �ƽ��� .�Ф�
	mutex_.lock();
	auto findSession = std::find(sessionList_.begin(), sessionList_.end(), session);
	if (findSession != sessionList_.end())
	{
		mutex_.unlock();
		return false;
	}
	if (sessiontCount_ > maxConnCount_) {
		mutex_.unlock();
		return false;
	}

	session->setSessionID(this->createSessionID());
	sessionList_.push_back(session);

	mutex_.unlock();
	return true;
}

list<Session*>& SessionManager::GetSessionList()
{
	return sessionList_;
}

bool SessionManager::closeSession(Session * session)
{
	mutex_.lock();
	if (session == nullptr) {
		mutex_.unlock();
		return false;
	}
	auto findSession = std::find(sessionList_.begin(), sessionList_.end(), session);
	if (findSession != sessionList_.end())
	{
		Session *deleteSession = *findSession;
		::closesocket(deleteSession->socket());
		sessionList_.remove(deleteSession);
		--sessiontCount_;
		SAFE_DELETE(deleteSession);
		mutex_.unlock();
		return true;
	}
	mutex_.unlock();
	return false;
}

void SessionManager::forceCloseSession(Session * session)
{
	mutex_.lock();
	if (!session) {
		mutex_.unlock();
		return;
	}
	LINGER linger;
	linger.l_onoff = 1;   //���
	linger.l_linger = 0;  //���ð�, 0�Ͻ� �Ϸ� �ȵ� ��Ŷ ������ ��� ����.

	::setsockopt(session->socket(), SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
	mutex_.unlock();
}

Session * SessionManager::session(int8_t id)
{
	mutex_.lock();
	Session *findSession = nullptr;

	for (auto session : sessionList_) {
		if (session->GetSessionId() == id) {
			findSession = session;
			break;
		}
	}
	mutex_.unlock();
	return findSession;
}
