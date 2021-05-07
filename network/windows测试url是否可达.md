```cpp
#include <stdio.h>
#include <stdlib.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <Wininet.h>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Wininet.lib")
#include <iostream>
#include <vector>
#include <string>
#include <tchar.h>
template <typename TString>
std::vector<TString> SplitString(TString& tstring, TString& delim) {
	if (tstring.empty()) return std::vector<TString>();

	typename TString::size_type start = 0;
	typename TString::size_type end = 0;
	typename TString::size_type size = tstring.size();
	std::vector<TString> retVal{};
	do
	{
		end = tstring.find(delim, start);
		if (end == TString::npos) end = size;
		if (start != end) retVal.emplace_back(tstring.substr(start, end - start));
		start = end + 1;
	} while (start < size);

	return std::move(retVal);
}

bool IsValidIP(const char* ip,UINT32 *uip) {
	std::string IP = ip;
	static std::string delim = ".";
	auto ip_vec = SplitString(IP, delim);
	if (ip_vec.size() == 4) {
		UINT32 result = 0;
		for (auto& it : ip_vec) {
			UINT32 seg = std::stoi(it);
			if (seg > 255) return false;
			result = (result << 8) + seg;
		}
		*uip = result;
		return true;
	}
	return false;
}

bool IsValidIP(const char* ip) {
	std::string IP = ip;
	static std::string delim = ".";
	auto ip_vec = SplitString(IP, delim);
	if (ip_vec.size() == 4) {
		for (auto& it : ip_vec) {
			UINT32 seg = std::stoi(it);
			if (seg > 255) return false;
		}
		return true;
	}
	return false;
}

ULONG ptonbyaddress(const char* address) {
	WSADATA wsaData;
	INT iRetval = 0;

	DWORD dwRetval;

	int i = 1;

	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	struct sockaddr_in* sockaddr_ipv4;

	char ipstringbuffer[46];
	DWORD ipbufferlength = 46;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		return iRetval;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	dwRetval = getaddrinfo(address, "", &hints, &result);
	if (dwRetval != 0) {
		WSACleanup();
		return iRetval;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		if (ptr->ai_family == AF_UNSPEC) {
			std::cout << "unspecified AF_FAMILY" << std::endl;
		}
		else if (ptr->ai_family == AF_INET) {
			sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
			iRetval = sockaddr_ipv4->sin_addr.S_un.S_addr;
			break;		
		}
		else if (ptr->ai_family == AF_INET6) {
			std::cout << "ipv6 not suported" << std::endl;
		}
		else {
			std::cout << "Other " << ptr->ai_family << ", not supported" << std::endl;
		}
	}
	freeaddrinfo(result);
	WSACleanup();
	return iRetval;
}

ULONG ptonbyip(const char* ip) {
	UINT32 uip = 0;
	UINT32 port = 0;
	SOCKADDR_IN sockaddr_in{};
	ULONG numeric = 0;
	inet_pton(AF_INET, ip, (void*)&numeric);
	return numeric;
}

ULONG pton(const char* presentation) {

	if (IsValidIP(presentation)) {
		return ptonbyip(presentation);
	}
	else {
		return ptonbyaddress(presentation);
	}
	
}

BOOL CheckConnection(const TCHAR * url)
{
	DWORD dwState = 0;
	DWORD dwSize = sizeof(DWORD);
	BOOL bConnect = FALSE;
	/*URL_COMPONENTS url_components{};
	TCHAR Scheme[INTERNET_MAX_SCHEME_LENGTH]{};
	TCHAR HostName[INTERNET_MAX_HOST_NAME_LENGTH]{};
	TCHAR UserName[INTERNET_MAX_USER_NAME_LENGTH]{};
	TCHAR Password[INTERNET_MAX_PASSWORD_LENGTH]{};
	TCHAR UrlPath[INTERNET_MAX_URL_LENGTH]{};
	TCHAR ExtraInfo[1000]{};

	url_components.dwStructSize = sizeof(url_components);
	url_components.lpszScheme = Scheme;
	url_components.lpszHostName = HostName;
	url_components.lpszUserName = UserName;
	url_components.lpszPassword = Password;
	url_components.lpszUrlPath = UrlPath;
	url_components.lpszExtraInfo = ExtraInfo;
	url_components.dwSchemeLength = INTERNET_MAX_SCHEME_LENGTH;
	url_components.dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;
	url_components.dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;
	url_components.dwPasswordLength = INTERNET_MAX_PASSWORD_LENGTH;
	url_components.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	url_components.dwExtraInfoLength = 1000;
	
	if (TRUE == InternetCrackUrl(url, 0, 0, &url_components)) {*/
		try
		{
			if (::InternetQueryOption(NULL, INTERNET_OPTION_CONNECTED_STATE, &dwState, &dwSize) &&
				(dwState & INTERNET_STATE_CONNECTED))
			{
				if (::InternetCheckConnection(url,
					FLAG_ICC_FORCE_CONNECTION, 0)) {
					bConnect = TRUE;
				}
				else {
					DWORD dwErrorCode = ::GetLastError();
					int n = 10;
				}
			}
		}
		catch (...)
		{
			bConnect = FALSE;
		}
		return bConnect;
	//}
	
}

int main(void)
{
	auto bConnect = CheckConnection(L"https://www.baidu.com");
	bConnect = CheckConnection(L"https://www.baidu.com:443");//只要是个url就行，不管带不带参数例如
	//https://www.baidu.com:443/?password=xxxxxx
	bConnect = CheckConnection(L"https://www.cnblogs.com/ider/archive/2011/07/22/cpp_cast_operator_part2.html");
	{
		SOCKADDR_IN sockaddr_in{};
		auto result = inet_pton(AF_INET, "127.123.133.166", (void*)&sockaddr_in.sin_addr);
		printf("inet_pton: 0x%x\n", sockaddr_in.sin_addr.s_addr); // 注意得到的字节序
		// 反转换
		CHAR ip[INET_ADDRSTRLEN]{};
		inet_ntop(AF_INET, (void*)&sockaddr_in.sin_addr, ip, 16);
		printf("inet_ntop: %s\n", ip);
	}
	
	SOCKADDR_IN sockaddr_in{};
	auto temp = pton("www.baidu.com");







}
```
