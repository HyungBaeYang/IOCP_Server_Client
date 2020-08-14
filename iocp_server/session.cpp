#include "stdafx.h"
#include "Session.h"

Session::Session()
{
	this->updateHeartbeat();

	// overlapped�� ó���� �׻� �о��ְ� ���.
	ZeroMemory(&recv_overlap.original_overlapped, sizeof(recv_overlap.original_overlapped));
}

Session::~Session()
{
	this->onClose();
}

bool Session::setSocketOpt()
{
	// ���� �ð� �����ϸ鼭 ���Ͽɼ� ����.
	tcp_keepalive keepAliveSet = { 0 }, returned = { 0 };
	keepAliveSet.onoff = 1;
	keepAliveSet.keepalivetime = 3000; // 3�� �������� �ð�.
	keepAliveSet.keepaliveinterval = 3000; // ��������� 3���Ŀ� �ٽ� ����.

	DWORD dwByte;
	if (WSAIoctl(socketData_.socket_, SIO_KEEPALIVE_VALS, &keepAliveSet, sizeof(keepAliveSet), &returned, sizeof(returned), &dwByte, NULL, NULL)) {
		return false;
	}

}

bool Session::onAccept(SOCKET socket, SOCKADDR_IN addrInfo)
{
	// ������ �����ϰ�, ���Ͽɼ� ����.
	socketData_.socket_ = socket;
	int addrLen;
	getpeername(socketData_.socket_, (struct sockaddr*)&socketData_.addrInfo_, &addrLen);
	socketData_.addrInfo_ = addrInfo;
	if (!this->setSocketOpt())
	{
		return false;
	}
	return true;
}

void Session::onClose(bool force)
{
	// ���ǸŴ������� force = true�̸� �Ŵ������� ��������.
	// �׷��������� �Ŵ������� ���� ����.
	if (force) {
		SessionManageMent.forceCloseSession(this);
	}
	else
	{
		SessionManageMent.closeSession(this);
	}
}


SOCKET& Session::socket()
{
	return socketData_.socket_;
}

int Session::GetSessionId()
{
	return id_;
}

void Session::setSessionID(unsigned int id)
{
	id_ = id;
}

OVERLAP_EX & Session::GetOverlapped()
{
	return recv_overlap;
}

int Session::GetPacketSize()
{
	return packet_size;
}

int Session::GetPreviousSize()
{
	return previous_size;
}

int8_t Session::GetSessionType()
{
	return type_;
}

time_t Session::Heartbeat()
{
	return lastHeartbeat_;
}

void Session::updateHeartbeat()
{
	lastHeartbeat_ = system_clock::to_time_t(system_clock::now());
}

void Session::SetPacketSize(int size)
{
	packet_size = size;
}

void Session::SetPreviosSize(int size)
{
	previous_size = size;
}
