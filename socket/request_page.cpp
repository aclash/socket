#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#include <windows.h>
#include <string>
#include <stdio.h>
#include <regex>
#include <vector>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

HINSTANCE hInst;
WSADATA wsaData;
void mParseUrl(char *mUrl, string &serverName, string &filepath, string &filename);
SOCKET connectToServer(char *szServerName, WORD portNum);
int getHeaderLength(char *content);
char *readUrl2(char *szUrl, long &bytesReturnedOut, char **headerOut);
char* formatHTML_start(char* str, long& len);
char* formatHTML_camp(char* str, long& len);
char* formatHTML_bbq(char* str, long& len);
char* formatHTML_beach(char* str, long& len);


const char *start = "http://csweb01.csueastbay.edu/~td4679/Pgm3/start";
const char *camping = "http://csweb01.csueastbay.edu/~td4679/Pgm3/camp.html";
const char *beach = "http://csweb01.csueastbay.edu/~td4679/Pgm3/beach.html";
const char *bbq = "http://csweb01.csueastbay.edu/~td4679/Pgm3/bbq.html";
vector<char*> UrlSrc{ (char*)start, (char*)camping, (char*)beach, (char*)bbq };

typedef char*(*T_pfun)(char*, long&); 
vector<T_pfun> pFunVec{ formatHTML_start, formatHTML_camp, formatHTML_beach, formatHTML_bbq };
enum page_state {
	START = 0,
	CAMPING = 1,
	BEACH = 2,
	BBQ = 3
};

page_state state = page_state::START;

void SetState(int index)
{
	printf("Your choice : %d\n", index);
	switch (state)
	{
		case page_state::START: 
		{ 
			UrlSrc[1] = (char*)camping;
			UrlSrc[2] = (char*)beach;
			UrlSrc[3] = (char*)bbq;
			if (index == 1)
				state = page_state::CAMPING;
			else if (index == 2)
				state = page_state::BEACH;
			else if (index == 3)
				state = page_state::BBQ;
			break; 
		}
		case page_state::CAMPING: 
		{
			UrlSrc[1] = (char*)bbq; 
			UrlSrc[2] = (char*)beach; 
			UrlSrc[3] = (char*)start; 
			if (index == 1)
				state = page_state::BBQ;
			else if (index == 2)
				state = page_state::BEACH;
			else if (index == 3)
				state = page_state::START;
			break; 
		}
		case page_state::BEACH: 
		{
			UrlSrc[1] = (char*)bbq; 
			UrlSrc[2] = (char*)camping; 
			UrlSrc[3] = (char*)start; 
			if (index == 1)
				state = page_state::BBQ;
			else if (index == 2)
				state = page_state::CAMPING;
			else if (index == 3)
				state = page_state::START;
			break; 
		}
		case page_state::BBQ: 
		{
			UrlSrc[1] = (char*)beach; 
			UrlSrc[2] = (char*)camping; 
			UrlSrc[3] = (char*)start;
			if (index == 1)
				state = page_state::BEACH;
			else if (index == 2)
				state = page_state::CAMPING;
			else if (index == 3)
				state = page_state::START;
			break; 
		}
		default:break;
	}
	printf("New url : %s\n", UrlSrc[index]);
	long fileSize;
	char *memBuffer, *headerBuffer;
	memBuffer = headerBuffer = NULL;

	memBuffer = readUrl2(UrlSrc[index], fileSize, &headerBuffer);
	char* res = NULL;
	res = pFunVec[state](memBuffer, fileSize);	//strip <tags> off
	printf("data returned:\n%s", res);
	printf("Enter a number[1 - 3] or q to quit :\n");

	if (fileSize != 0)
	{
		delete(res);
		delete(headerBuffer);
	}
}

int main()
{
	long fileSize;
	char *memBuffer, *headerBuffer;
	FILE *fp;
	
	memBuffer = headerBuffer = NULL;

	if (WSAStartup(0x101, &wsaData) != 0)
		return -1;

	memBuffer = readUrl2(UrlSrc[0], fileSize, &headerBuffer);
	char* res = NULL;
	res = formatHTML_start(memBuffer, fileSize);	//strip <tags> off
	printf("data returned:\n%s", res);

	if (fileSize != 0)
	{
		//printf("Got some data, write into file\n");
		//fp = fopen("downloaded.file", "wb");
		//fwrite(res, 1, fileSize, fp);
		//fclose(fp);
		delete(res);
		delete(headerBuffer);
	}
	

	printf("Enter a number[1 - 3] or q to quit :\n");
	char input;
	while (scanf("%c", &input) && input != 'q')
	{
		if (input == '1')
		{
			SetState(1);
		}
		else if (input == '2')
		{
			SetState(2);
		}
		else if (input == '3')
		{
			SetState(3);
		}
	}
	printf("Exit\n");
	WSACleanup();
	system("pause");
	return 0;
}

void mParseUrl(char *mUrl, string &serverName, string &filepath, string &filename)
{
	string::size_type n;
	string url = mUrl;
	if (url.substr(0, 7) == "http://")
		url.erase(0, 7);
	if (url.substr(0, 8) == "https://")
		url.erase(0, 8);
	n = url.find('/');
	if (n != string::npos)
	{
		serverName = url.substr(0, n);
		filepath = url.substr(n);
		n = filepath.rfind('/');
		filename = filepath.substr(n + 1);
	}
	else
	{
		serverName = url;
		filepath = "/";
		filename = "";
	}
}

SOCKET connectToServer(char *szServerName, WORD portNum)
{
	struct hostent *hp;
	unsigned int addr;
	struct sockaddr_in server;
	SOCKET conn;

	conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn == INVALID_SOCKET)
		return NULL;

	if (inet_addr(szServerName) == INADDR_NONE)
	{
		hp = gethostbyname(szServerName);
	}
	else
	{
		addr = inet_addr(szServerName);
		hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	}
	if (hp == NULL)
	{
		closesocket(conn);
		return NULL;
	}
	server.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(portNum);
	if (connect(conn, (struct sockaddr*)&server, sizeof(server)))
	{
		closesocket(conn);
		return NULL;
	}
	return conn;
}

int getHeaderLength(char *content)
{
	const char *srchStr1 = "\r\n\r\n", *srchStr2 = "\n\r\n\r";
	char *findPos;
	int ofset = -1;

	findPos = strstr(content, srchStr1);
	if (findPos != NULL)
	{
		ofset = findPos - content;
		ofset += strlen(srchStr1);
	}
	else
	{
		findPos = strstr(content, srchStr2);
		if (findPos != NULL)
		{
			ofset = findPos - content;
			ofset += strlen(srchStr2);
		}
	}
	return ofset;
}

char* formatHTML_start(char* str, long& len)
{
	string html(str);
	delete(str);
	std::regex tags("<p>");
	std::string output1;
	std::regex_replace(std::back_inserter(output1), html.begin(), html.end(), tags, "\n");
	tags = ("<[^<]*>");
	std::string output2;
	std::regex_replace(std::back_inserter(output2), output1.begin(), output1.end(), tags, "");
	tags = ("Camping");
	output1.clear();
	std::regex_replace(std::back_inserter(output1), output2.begin(), output2.end(), tags, "[1]. Camping");

	tags = ("Beach");
	output2.clear();
	std::regex_replace(std::back_inserter(output2), output1.begin(), output1.end(), tags, "[2]. Beach");

	tags = ("Barbeque");
	output1.clear();
	std::regex_replace(std::back_inserter(output1), output2.begin(), output2.end(), tags, "[3]. Barbeque");

	len = output1.length();
	char *cstr = new char[output1.length() + 1];
	strcpy(cstr, output1.c_str());
	return cstr;
}


char* formatHTML_camp(char* str, long& len)
{
	string html(str);
	delete(str);
	std::regex tags("<p>");
	std::string output1;
	std::regex_replace(std::back_inserter(output1), html.begin(), html.end(), tags, "\n");
	tags = ("<[^<]*>");
	std::string output2;
	std::regex_replace(std::back_inserter(output2), output1.begin(), output1.end(), tags, "");
	tags = ("BBQ");
	output1.clear();
	std::regex_replace(std::back_inserter(output1), output2.begin(), output2.end(), tags, "[1]. BBQ");

	tags = ("Beach");
	output2.clear();
	std::regex_replace(std::back_inserter(output2), output1.begin(), output1.end(), tags, "[2]. Beach");

	tags = ("Back to Start");
	output1.clear();
	std::regex_replace(std::back_inserter(output1), output2.begin(), output2.end(), tags, "[3]. Back to Start");

	len = output1.length();
	char *cstr = new char[output1.length() + 1];
	strcpy(cstr, output1.c_str());
	return cstr;
}


char* formatHTML_bbq(char* str, long& len)
{
	string html(str);
	delete(str);
	std::regex tags("<p>");
	std::string output1;
	std::regex_replace(std::back_inserter(output1), html.begin(), html.end(), tags, "\n");
	tags = ("<[^<]*>");
	std::string output2;
	std::regex_replace(std::back_inserter(output2), output1.begin(), output1.end(), tags, "");
	tags = ("Beach");
	output1.clear();
	std::regex_replace(std::back_inserter(output1), output2.begin(), output2.end(), tags, "\n[1]. Beach");

	tags = ("Camping");
	output2.clear();
	std::regex_replace(std::back_inserter(output2), output1.begin(), output1.end(), tags, "[2]. Camping");

	tags = ("Back to Start");
	output1.clear();
	std::regex_replace(std::back_inserter(output1), output2.begin(), output2.end(), tags, "[3]. Back to Start");

	len = output1.length();
	char *cstr = new char[output1.length() + 1];
	strcpy(cstr, output1.c_str());
	return cstr;
}


char* formatHTML_beach(char* str, long& len)
{
	string html(str);
	delete(str);
	std::regex tags("<p>");
	std::string output1;
	std::regex_replace(std::back_inserter(output1), html.begin(), html.end(), tags, "\n");
	tags = ("<[^<]*>");
	std::string output2;
	std::regex_replace(std::back_inserter(output2), output1.begin(), output1.end(), tags, "");
	tags = ("Have a BBQ");
	output1.clear();
	std::regex_replace(std::back_inserter(output1), output2.begin(), output2.end(), tags, "\n[1]. Have a BBQ");

	tags = ("Go camping");
	output2.clear();
	std::regex_replace(std::back_inserter(output2), output1.begin(), output1.end(), tags, "[2]. Go camping");

	tags = ("Back to Start");
	output1.clear();
	std::regex_replace(std::back_inserter(output1), output2.begin(), output2.end(), tags, "[3]. Back to Start");

	len = output1.length();
	char *cstr = new char[output1.length() + 1];
	strcpy(cstr, output1.c_str());
	return cstr;
}

char *readUrl2(char *szUrl, long &bytesReturnedOut, char **headerOut)
{
	const int bufSize = 512;
	char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
	char *tmpResult = NULL, *result;
	SOCKET conn;
	string server, filepath, filename;
	long totalBytesRead, thisReadSize, headerLen;

	mParseUrl(szUrl, server, filepath, filename);

	///////////// step 1, connect //////////////////////
	conn = connectToServer((char*)server.c_str(), 80);

	///////////// step 2, send GET request /////////////
	sprintf(tmpBuffer, "GET %s HTTP/1.0", filepath.c_str());
	strcpy(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	sprintf(tmpBuffer, "Host: %s", server.c_str());
	strcat(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	strcat(sendBuffer, "\r\n");
	send(conn, sendBuffer, strlen(sendBuffer), 0);

	//    SetWindowText(edit3Hwnd, sendBuffer);
	printf("Buffer being sent:\n%s", sendBuffer);

	///////////// step 3 - get received bytes ////////////////
	// Receive until the peer closes the connection
	totalBytesRead = 0;
	while (1)
	{
		memset(readBuffer, 0, bufSize);
		thisReadSize = recv(conn, readBuffer, bufSize, 0);

		if (thisReadSize <= 0)
			break;

		tmpResult = (char*)realloc(tmpResult, thisReadSize + totalBytesRead);

		memcpy(tmpResult + totalBytesRead, readBuffer, thisReadSize);
		totalBytesRead += thisReadSize;
	}

	headerLen = getHeaderLength(tmpResult);
	long contenLen = totalBytesRead - headerLen;
	result = new char[contenLen + 1];
	memcpy(result, tmpResult + headerLen, contenLen);
	result[contenLen] = 0x0;
	char *myTmp;

	myTmp = new char[headerLen + 1];
	strncpy(myTmp, tmpResult, headerLen);
	myTmp[headerLen] = NULL;
	delete(tmpResult);
	*headerOut = myTmp;

	bytesReturnedOut = contenLen;
	closesocket(conn);
	return(result);
}