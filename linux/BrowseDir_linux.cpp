#include "BrowseDir_linux.h"

CBrowseDirLinux::CBrowseDirLinux()
{
    //用当前目录初始化m_szInitDir
    getcwd(m_szInitDir, _MAX_PATH);

    //如果目录的最后一个字母不是'\',则在最后加上一个'\'
    size_t len = strlen(m_szInitDir);
    if (m_szInitDir[len - 1] != '/')
        strcat(m_szInitDir, "/");
}

bool CBrowseDirLinux::SetInitDir(const char *dir)
{
    memset(m_szInitDir, '\0', _MAX_PATH);

    if (realpath(dir, m_szInitDir) == NULL)
        return false;
    
    printf("%s\n", m_szInitDir);
    //判断目录是否存在
    if (chdir(m_szInitDir) != 0)
        return false;

    return true;
}

std::vector<std::string> CBrowseDirLinux::BeginBrowseFilenames(const char *filespec)
{
    ProcessDir(m_szInitDir, NULL);
    return GetDirFilenames(m_szInitDir, filespec);
}

std::vector<std::string> CBrowseDirLinux::BeginBrowseFilenames(const char* dir_name,
    const char *filespec)
{
    return GetDirFilenames(dir_name, filespec);
}

bool CBrowseDirLinux::BeginBrowse(const char *filespec)
{
    ProcessDir(m_szInitDir, NULL);
    return BrowseDir(m_szInitDir, filespec);
}

bool CBrowseDirLinux::FilespecFilter(const char* file_name, const char* filespec)
{
    std::string file_spec = std::string(filespec);
    std::size_t found = file_spec.find_last_of(".");
    file_spec = file_spec.substr(found);

    std::string filename = std::string(file_name);
    found = filename.find_last_of(".");
    std::string spec("");
    if (found != std::string::npos)
    {
        std::string spec = filename.substr(found);
    }

	if (spec == file_spec || file_spec == std::string(".*"))
    {
        return true;
    }
    return false;
}

bool CBrowseDirLinux::BrowseDir(const char *dir, const char *filespec)
{
    DIR* dp;
    struct dirent *entry;
    struct stat statbuf;

    if((dp = opendir(dir)) == NULL)
    {
        printf("Can't open directory %s\n", dir);
        return false;
    };

    chdir(dir);

    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);

        if(S_ISDIR(statbuf.st_mode))
        {
            if(strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            std::string dir_name = dir;

            if(dir_name.back() == '/')
            {
                dir_name = dir_name + entry->d_name;
            }
            else
            {
                dir_name = dir_name + '/' + entry->d_name;
            }
            ProcessDir(dir_name.c_str(), filespec);
            BrowseDir((char*)dir_name.c_str(), filespec);
        }
        else
        {
            std::string file_name = dir;
            if(file_name.back() == '/')
            {
                file_name = file_name + entry->d_name;
            }
            else
            {
                file_name = file_name + '/' + entry->d_name;
            }

            if(FilespecFilter(file_name.c_str(), filespec))
            {
                //printf("%s\n", file_name.c_str());
                if(!ProcessFile(file_name.c_str()))
                    return false;
            }
        }
    }

    chdir("..");

    closedir(dp);
    getcwd(m_szInitDir, _MAX_PATH);
    printf("current: %s\n", m_szInitDir);

    return true;
}

std::vector<std::string> CBrowseDirLinux::GetDirFilenames(const char *dir, const char *filespec)
{
    std::vector<std::string>filename_vector;
#if 1
    DIR* dp;
    struct dirent *entry;
    struct stat statbuf;

    if((dp = opendir(dir)) == NULL)
    {
        printf("Can't open directory %s\n", dir);
        return filename_vector;
    };

    chdir(dir);

    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);

        if(S_ISDIR(statbuf.st_mode))
        {
            if(strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            std::string dir_name = dir;

            if(dir_name.back() == '/')
            {
                dir_name = dir_name + entry->d_name;
            }
            else
            {
                dir_name = dir_name + '/' + entry->d_name;
            }
            BrowseDir((char*)dir_name.c_str(), filespec);
        }
        else
        {
            std::string file_name = dir;
            if(file_name.back() == '/')
            {
                file_name = file_name + entry->d_name;
            }
            else
            {
                file_name = file_name + '/' + entry->d_name;
            }

            if(FilespecFilter(file_name.c_str(), filespec))
            {
                printf("%s\n", file_name.c_str());
                filename_vector.push_back(file_name);
            }
        }
    }

    chdir("..");

    closedir(dp);
#endif
    return filename_vector;
}

bool CBrowseDirLinux::ProcessFile(const char *filename)
{
    return true;
}

void CBrowseDirLinux::ProcessDir(const char
                            *currentdir, const char *parentdir)
{
    
}

#if 0
int main()
{
    CBrowseDirLinux dir;
    dir.SetInitDir("./trunk/");
    dir.BeginBrowse("*.sh");

    dir.SetInitDir("./trunk");
    std::vector<std::string> files_name = dir.BeginBrowseFilenames("*.cpp");
    for (auto name : files_name)
    {
        printf("%s\n", name.c_str());
    }
}
#endif
