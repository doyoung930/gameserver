#include "Core.h"
#include"Server.h"
Core* Core::m_pInst = NULL;
Core::Core()
{
}

Core::~Core()
{
}

bool Core::Init()
{
    if (!Server::GetInst()->Init())
        return false;
    return true;
}

int Core::Run()
{
    Server::GetInst()->Run();

    return 0;
}
