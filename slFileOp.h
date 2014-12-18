///Basic File Operations used by SL
///=====================================================
///This program is designed to be extended.(extended by reindex)
///Note:Some of the function is modified.
///=====================================================
///Programed by SuperLucky
///2012-12-5 with Code::Block

#ifndef SLFileOp_H
#define SLFileOP_H

#include <stdio.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>
#include <string>
#include <vector>

#define BUFFER_SIZE 256

using namespace std;

int slFindFiles(string path, string extension, vector<string>& files);
string slGetExtension(string path);
string slGetFileNameWithoutExtension(string fileName);
bool slMatchExtension(string fileName,string extension);
string slGetRelativePath(string Path,string PathBase);
string slGetFileDirectory(string Path);
void slCopyFile(string src,string dst);
void slShowVector(vector<string> v);

/* Sample of usage.
vector<string> Files;

int main()
{
    DWORD driverMask=0;//盘符掩码
    char driverName[3]="A:";//起始盘符的名字（A：-Z:）

    driverMask=GetLogicalDrives();//返回系统中有的盘符，这个32位数的每一位代表一个盘符，最低位是A;
    int mask=1;//检验当前的盘符，初始化为A:
    string rootDriver;//记录driverName.
    while(mask<0x100000)//检差24位。
    {
        if(mask&driverMask)//当前选择的盘符存在
        {
            rootDriver=driverName;
            cout<<"Searching on "<<rootDriver<<"...";
            findFiles(rootDriver,"txt",Files);
            cout<<"success."<<endl;
        }
        mask<<=1;//mask移位，检测下一个盘符。
        driverName[0]++;//盘符递增
    }

    showVector(Files);

    printf("Finished\n");
    system("PAUSE");
    return 0;
}*/

///Get the file paths in given path by givern extensions recuisivly.
///path: The initial finding path.
///extension: File extensions to math. Mutiple extensions can be seperated by '|', like txt|xml|ini
///files: Return file pathes that found.
///Return: the number of files found.
int slFindFiles(string path, string extension, vector<string>& files)
{
    long hFile=0L;
    _finddata_t fileInfo;
    string p;
    if((hFile=_findfirst(p.assign(path).append("/*").c_str(),&fileInfo))!=-1L)
    {
        do
        {
            if(fileInfo.attrib & _A_SUBDIR)
            {
                if(strcmp(fileInfo.name,".") && strcmp(fileInfo.name,".."))
                    slFindFiles(p.assign(path).append("\\").append(fileInfo.name), extension, files);
            }
            else
            {
                if(slMatchExtension(fileInfo.name, extension))
                {
                    string value = p.assign(path).append("\\").append(fileInfo.name);
                    files.push_back(value);
                }
            }
        }
        while(_findnext(hFile, &fileInfo)==0);
        _findclose(hFile);
    }

    return files.size();
}

///Get current file extension. If the file got no extensions, return empty string.
///Return: extension of the file.
///i.e. C:\1.txt as path, then return txt.
string slGetExtension(string path)
{
    int p=path.length()-1;

    while(path[p]!='.' && p>=0) --p;

    if(p>=0) return path.substr(p+1);
    else return "";
}

///Match the fileName to the given extensions
///fileName: the fileName or fullPath with extension
///extension: a '|' seperate string for accecptable extensions, e.g. xml|txt|ini, * for anyextension
///return: if the extension of the file is one of the extenstions return true otherwise false;
bool slMatchExtension(string fileName,string extension)
{
    char *str = (char *) malloc(extension.length()+1);
    char *ext = (char *) malloc(slGetExtension(fileName).length()+1);
    strcpy(str,extension.c_str());
    strcpy(ext,slGetExtension(fileName).c_str());

    bool isMatch = false;
    char *p = strtok(str,"|");

    while(p && !isMatch)
    {
        if(strcmp(ext,"*")==0) isMatch=true;
        else if (strcmp(ext,p)==0) isMatch=true;
        else p = strtok(NULL,"|");
    }

    free(str);
    free(ext);

    return isMatch;
}

///Get the filename with out extension and pre-directories.
///fileName: the fileName or fullPath with extension
///return: the raw filename e.g. C:\HAHA\WAWA.txt->WAWA
string slGetFileNameWithoutExtension(string fileName)
{
    int ph,pt;

    ph=-1;
    pt=fileName.length()-1;

    for(int i=0;i<=fileName.length();i++)
        if(fileName[i]=='/' || fileName[i]=='\\') ph=i;
    for(int i=fileName.length()-1; i>=0; i--)
        if(fileName[i]=='.') {pt=i; break;}
    return fileName.substr(ph+1, pt-ph-1);
}

///Get the relative path for output
///stringPath: must be the XMLSourcePath as a sub
///return: the relative path for output,
///e.g. C:\XML\cate1\cate2/3.xml(XML as the XMLSourcePath)->cate1\cate2
///note that each '/' is replaysed by '\';
string slGetRelativePath(string Path,string PathBase)
{
    int sta = Path.find(PathBase);
    int pos = Path.length()-1;

    if(sta==string::npos)
    {
        printf("Path is not containing the PathBase\n");
        exit(0);
    }
    sta+=PathBase.length();
    while(pos>=0 && Path[pos]!='/' && Path[pos]!='\\') pos--;
    if(pos<sta)
    {
        printf("Error occored in finding relativePath\n");
        exit(0);
    }
    if(pos==sta) return "";
    return Path.substr(sta+1,pos-sta-1);
}

///Copy file form src to dst, with overwrite
///src: Source file.
///dst: Target file, if target file exists overwite, else create new.
void slCopyFile(string src,string dst)
{
    FILE *in_file, *out_file;
    char data[BUFFER_SIZE];

    string outPath=slGetFileDirectory(dst);
    if(access(outPath.c_str(),0)==-1)
    {
        string cmd="md \"";
        cmd.append(outPath).append("\"");
        system(cmd.c_str());
    }

    in_file=fopen(src.c_str(),"rb");
    out_file=fopen(dst.c_str(),"wb");
    if(!in_file)
    {
        printf("Can not read %s",src.c_str());
        if(out_file) fclose(out_file);
        return;
    }
    if(!out_file)
    {
        printf("Can not write %s",src.c_str());
        if(in_file) fclose(in_file);
        return;
    }

    int datalen=0;
    while((datalen=fread(data,1,BUFFER_SIZE,in_file))>0) fwrite(data,1,datalen,out_file);
    fflush(out_file);
    fclose(out_file);
    fclose(in_file);
}


///Get the Directory of a file path
///path: Target file path.
///Return: The Directory where the path is.
///i.e. C:\ABC\a.txt as path, returns C:\ABC
string slGetFileDirectory(string Path)
{
    int p=Path.length()-1;
    while(p>=0 && Path[p]!='\\' && Path[p]!='/') p--;
    if(p<0) return "";
    else return Path.substr(0,p);
}

///Print the vector<string>
void slShowVector(vector<string> v)
{
    vector<string>::iterator itr;
    for(itr=v.begin();itr!=v.end();itr++)
        printf("%s\n",itr->c_str());
}
#endif
