#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>

#ifndef _USE_OLD_IOSTREAMS
using namespace std;
#endif

#define READ_FD 0
#define WRITE_FD 1

#define CHECK(a) if ((a)!= 0) return -1;

class StdOutRedirect
{
    public:
        StdOutRedirect(int bufferSize);
        ~StdOutRedirect();

        int Start();
        int Stop();
        int GetBuffer(char *buffer, int size);

    private:
        int fdStdOutPipe[2];
        int fdStdOut;
};

StdOutRedirect::~StdOutRedirect()
{
    _close(fdStdOut);
    _close(fdStdOutPipe[WRITE_FD]);
    _close(fdStdOutPipe[READ_FD]);
}
StdOutRedirect::StdOutRedirect(int bufferSize)
{
    if (_pipe(fdStdOutPipe, bufferSize, O_TEXT)!=0)
    {
        //treat error eventually
    }
    fdStdOut = _dup(_fileno(stdout));
}

int StdOutRedirect::Start()
{
    fflush( stdout );
    CHECK(_dup2(fdStdOutPipe[WRITE_FD], _fileno(stdout)));
    ios::sync_with_stdio();
    setvbuf( stdout, NULL, _IONBF, 0 ); // absolutely needed
    return 0;
}

int StdOutRedirect::Stop()
{
    CHECK(_dup2(fdStdOut, _fileno(stdout)));
    ios::sync_with_stdio();
    return 0;
}

int StdOutRedirect::GetBuffer(char *buffer, int size)
{
    int nOutRead = _read(fdStdOutPipe[READ_FD], buffer, size);
    buffer[nOutRead] = '\0';
    return nOutRead;
}