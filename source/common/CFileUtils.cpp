#include "CFileUtils.h"

#ifdef WIN32
	#include <direct.h>
	#include <io.h>
	#include <windows.h>
#else
	#include "CTimeUtils.h"
	#include "CFileUtils.h"
	#include <stdlib.h>
	#include <locale.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <dirent.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#ifdef Mx870
		#include <svc.h>
	#endif
#endif

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

	CFileUtils::CFileUtils()
	{
	}
	
	CFileUtils::~CFileUtils()
	{
	}
	
#ifdef WIN32
	const int CFileUtils::MAX_PATH_SIZE= MAX_PATH;

	bool CFileUtils::getFilePath(const char* fullFileName, char* filePath)
	{

		bool result = false;
		char fileName[CFileUtils::MAX_PATH_SIZE] = {0};
		char fileExtension[CFileUtils::MAX_PATH_SIZE] = {0};

		//calling method that evaluates the file path and split its parts.
		result = breakFileName(fullFileName, filePath, fileName, fileExtension);

		return(result);

	}

	bool CFileUtils::getFileName(const char* fullFileName, char* fileName)
	{

		bool result = false;
		char filePath[CFileUtils::MAX_PATH_SIZE] = {0};
		char fileExtension[CFileUtils::MAX_PATH_SIZE] = {0};

		//calling method that evaluates the file path and split its parts.
		if((result = breakFileName(fullFileName, filePath, fileName, fileExtension)) == true)
		{
			if (strlen(fileExtension) > 0)
			{
				strcat(fileName, ".");
				strcat(fileName, fileExtension);
			}
		}
		
		return(result);

	}

	bool CFileUtils::getFileExtension(const char* fullFileName, char* fileExtension)
	{

		bool result = false;
		char filePath[CFileUtils::MAX_PATH_SIZE] = {0};
		char fileName[CFileUtils::MAX_PATH_SIZE] = {0};

		//calling method that evaluates the file path and split its parts.
		result = breakFileName(fullFileName, filePath, fileName, fileExtension);

		return(result);

	}

	bool CFileUtils::breakFileName(const char* fullFileName, char* filePath, char* fileName, char* fileExtension)
	{

		char* token = NULL;
		char* lastToken = NULL;

		//Locate the last ocurrence of "\"
		token = (char*) strpbrk(fullFileName, "\\");
		if (token == NULL)
		{
			token = (char*) strpbrk(fullFileName, "/");
			if (token == NULL)
			{
				return false;
			}
		}

		do
		{
			lastToken = token + 1;
			token = strpbrk(token + 1, "\\");
			if (token == NULL)
			{
				token = strpbrk(lastToken, "/" );
			}
		} while(token != NULL);
		strncpy(filePath, fullFileName , (lastToken - fullFileName));

		int len = (lastToken - fullFileName);
		if (len > 0)
		{
			filePath[ len - 1 ]= '\0';
		}
		else
		{
			filePath[ 0 ]= '\0';
		}

		strcpy(fileName, lastToken);

		token = strpbrk(fileName, ".");
		if(token != NULL)
		{
			do
			{
				lastToken = token;
				token = strpbrk(token + 1, ".");
			}while(token != NULL);

			if (lastToken)
			{
				strcpy(fileExtension, lastToken + 1);
				*lastToken = '\0';
			}
			else
			{
				fileExtension[ 0 ] = '\0';
			}
		}
		
		return true;
	}

	//his method extract one level of directory of the Path provided
	///Ex : path   = "c:\test\dir1\abc"
	///     return = "c:\test\dir1"
	///It returns false if couldnot extract the dir
	bool CFileUtils::extractDirectory(const char* path, char* truncatedPath)
	{
            bool ret = false;

            if (path == NULL || truncatedPath == NULL)
            {
                    return ret;
            }

            CBasicString auxPath(path);
            int idx = auxPath.lastIndexOfAny("\\/");
            truncatedPath[0] = '\0';
            if(idx > 0)
            {
                    ret = true;
                    strncpy( truncatedPath, path , idx );
            }

            return ret;
	}

	//reate the directorty specified by "path", and all the directories needed
	///in the path, if they do not exist
	void CFileUtils::createDirectoryRecursive( const char* path )
	{
		if (path == NULL || path[0] == '\0')
		{
			return;
		}


		CBasicString bslTmpPath;
		bslTmpPath.append(path);
		char* tokenSlash = (char*) strchr( path, '/' );
		if (tokenSlash != NULL)
		{
			bslTmpPath.replace("/","\\");
		}

		//temporary path
		char tmpPath[MAX_PATH_SIZE]= {'\0'};
		strcpy(tmpPath, bslTmpPath.cStr());

		//Hold the path, minus the last directory
		char truncatedPath[MAX_PATH_SIZE]= {'\0'};

		//Locate the first ocurrence of "\" ( should be in c:\\dir.... )
		char* token= (char*) strchr( tmpPath, '\\' );

		if (token == NULL)
		{
			//nao achou barra alguma
			return;
		}

		////Locate the first ocurrence of "\" ( should be in c:\\dir.... )
		//char* token= (char*) strchr( path, '\\' );
		//char* token2 = (char*) strchr( path, '/' );

		//if (token == NULL && token2 == NULL)
		//{
		//	//nao achou nenhuma das barras
		//	return;
		//}
		//else if (token == NULL && token2 != NULL)
		//{
		//	//achou apenas a segunda
		//	token = token2;
		//}
		//else if (token != NULL && token2 != NULL)
		//{
		//	//achou as duas pega a primeira
		//	if (token2 < token)
		//	{
		//		token = token2;
		//	}
		//}

		//Verify if the directory exist�s
		///zero     = exists
		///non zero = does�t exists
		if( access( tmpPath, 0 ) != 0 ){

			//Extract the last level of the directory tree
			CFileUtils::extractDirectory( tmpPath, truncatedPath );

			//Call recursively this method, in order to create
			///the deeper directories first
			CFileUtils::createDirectoryRecursive( truncatedPath );

			//Finally create the directory
			CreateDirectory( tmpPath, NULL );

		};//if( access( path, 0 ) != 0 )

	}

	bool CFileUtils::fileExists (const char* path)
	{
		///zero     = exists
		///non zero = does�t exists
		return (access( path , 0 ) == 0); 
	}

	bool CFileUtils::isDirectory (const char* path)
	{
		bool ret = false;
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		if (path != NULL)
		{
			//carrega a estrutura com informa��es do arquivo
			hFind = FindFirstFile(path , &findFileData);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				//verificamos se � ou nao um diretorio
				ret = isDirectory(&findFileData);
			}
			FindClose(hFind);
		}
		return ret;
	}

	bool CFileUtils::isDirectory (WIN32_FIND_DATA* findFileData)
	{
		bool ret = false;

		if (findFileData != NULL)
		{
			if ((findFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				ret = true;
			}
		}
		return ret;
	}

	bool CFileUtils::deleteFile (const char* path)
	{
		
		SetFileAttributes(path, FILE_ATTRIBUTE_NORMAL);

		///zero     = Ok
		///non zero = Error
		bool ret = remove(path) == 0;
		return ret;
	}

	void CFileUtils::deleteDirectory (const char* path)
	{
		if (CFileUtils::fileExists(path) == false)
		{
			return ;
		}
		
		const char* pStr = NULL;
		CBasicStringList allFiles;
		CFileUtils::listFileNames (path, "*", false, allFiles, EFOAll);

		int len = allFiles.count();

		for (int i = 0 ; i < len  ; i++)
		{
			pStr = allFiles.elmtAt(i);

			if (CFileUtils::isDirectory(pStr))
			{
				deleteDirectory (pStr);
			}
			else
			{
				CFileUtils::deleteFile(pStr);
			}
		}
		rmdir(path);
	}

	bool CFileUtils::copyFile (const char* path, const char* toPath)
	{
		///non zero	= Ok
		///zero		= Error

                CBasicString winPath(path);
                winPath.replace('/', '\\');

                CBasicString winToPath(toPath);
                winToPath.replace('/', '\\');

                ///Forçamos para não falhar se o arquivo existir
                return (::CopyFile(winPath.cStr(), winToPath.cStr(), FALSE) != 0);
	}

	bool CFileUtils::moveFile (const char* fromPath, const char* toPath, bool overwrite)
	{
            CBasicString winPath(fromPath);
            winPath.replace('/', '\\');

            CBasicString winToPath(toPath);
            winToPath.replace('/', '\\');

            //verifica se eh para sobrescrever
            if (overwrite)
            {
                    deleteFile(winToPath.cStr());
            }

            ///non zero	= Error
            ///zero		= OK
            return (rename(winPath.cStr(), winToPath.cStr()) == 0);
	}

	long CFileUtils::fileLength(const char* fullPath)
	{
		long ret = 0;

		//vamos abrir o arquivo para verificar o tamanho
		FILE* fp = fopen (fullPath, "r");
		if (fp)
		{
			//vamos para o final
			fseek ( fp, 0, SEEK_END );
			//pegamos a posi��o atual que � o mesmo que o tamanho
			ret = ftell( fp );

			fclose (fp);
		}

		return ret;
	}

	void CFileUtils::listFileNames ( const char* basePath, bool recursive, CBasicStringList& filesList, EFindOptions findOptions, bool fullPath)
	{
		CFileUtils::listFileNames ( basePath, "*",  recursive, filesList, findOptions, fullPath);	
	}

	void CFileUtils::listFileNames ( const char* basePath, const char* searchCriterion,  bool recursive, CBasicStringList& filesList, EFindOptions findOptions, bool fullPath)
	{
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		char path[MAX_PATH_SIZE] = {0};
		int basePathLen;

		basePathLen = strlen (basePath);

		//monta o criterio de busca, se nao tem '\' entao coloca
		if (basePath[basePathLen - 1] == '\\')
			sprintf (path, "%s%s", basePath, searchCriterion);
		else
			sprintf (path, "%s\\%s", basePath, searchCriterion);

		//carrega estrutura com informa��es do arquivo
		hFind = FindFirstFile(path , &findFileData);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				//verifica se � recursivo, se for, entao verificamos se � um diretorio
				if (recursive && isDirectory(&findFileData))
				{
					//se for recursivo e � um diret�rio, ainda temos que ver se n�o eh .
					if(strcmp(findFileData.cFileName, ".") != 0 &&
							strcmp(findFileData.cFileName, "..") != 0)
					{
						//concatena no nome do diretorio no basePath
						if (basePath[basePathLen - 1] == '\\')
							sprintf (path, "%s%s", basePath, findFileData.cFileName);
						else
							sprintf (path, "%s\\%s", basePath, findFileData.cFileName);

						if (findOptions == EFOAll || findOptions == EFOOnlyDirectories)
						{
							if (fullPath)
							{
								filesList.add(path);
							}
							else
							{
								filesList.add(path + basePathLen + 1);
							}					
							
							//filesList.add(path);
						}

						listFileNames (path, searchCriterion, true, filesList, findOptions);
					}
				}
				else
				{
					if (isDirectory(&findFileData))
					{
						//nao � recursivo e � diretorio

						//se for diretorio e for "." nao inserimos na lista
						if(strcmp(findFileData.cFileName, ".") !=0 &&
							strcmp(findFileData.cFileName, "..") != 0)
						{
							//nao � ".", eh nome de diretorio
							if (findOptions == EFOAll || findOptions == EFOOnlyDirectories)
							{
								sprintf(path, "%s\\%s", basePath, findFileData.cFileName);

								if (fullPath)
								{
									filesList.add(path);
								}
								else
								{
									filesList.add(path + basePathLen + 1);
								}					
							}
						}					
					}
					else
					{
						//n�o � recursivo e � arquivo

						//� arquivo
						if (findOptions == EFOAll || findOptions == EFOOnlyFiles)
						{
							sprintf(path, "%s\\%s", basePath, findFileData.cFileName);
							if (fullPath)
							{
								filesList.add(path);
							}
							else
							{
								filesList.add(path + basePathLen + 1);
							}					
							
							//filesList.add(path);
						}
					}
				}

			}while (FindNextFile(hFind, &findFileData) != 0);

			FindClose(hFind);
		}
	}

	char* CFileUtils::getCurrentDirectory (char* path, int pathLength)
	{
		if (path)
		{
			::GetCurrentDirectory (pathLength, path);
		}

		return path;
	}

	void CFileUtils::getFileAttributes (const char* path, CFileAttributes* fAttributes)
	{
		if (path)
		{
			DWORD attr = 0;
			attr =  GetFileAttributes (path);

			if (attr == 0)
			{
				fAttributes->archive		= (attr & FILE_ATTRIBUTE_ARCHIVE) == FILE_ATTRIBUTE_ARCHIVE;
				fAttributes->compressed		= (attr & FILE_ATTRIBUTE_COMPRESSED) == FILE_ATTRIBUTE_COMPRESSED;
				fAttributes->directory		= (attr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
				fAttributes->encripted		= (attr & FILE_ATTRIBUTE_ENCRYPTED) == FILE_ATTRIBUTE_ENCRYPTED;
				fAttributes->hidden			= (attr & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN;
				fAttributes->normal			= (attr & FILE_ATTRIBUTE_NORMAL) == FILE_ATTRIBUTE_NORMAL;
				fAttributes->readOnly		= (attr & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY;
				fAttributes->system			= (attr & FILE_ATTRIBUTE_SYSTEM) == FILE_ATTRIBUTE_SYSTEM;
				fAttributes->temporary		= (attr & FILE_ATTRIBUTE_TEMPORARY) == FILE_ATTRIBUTE_TEMPORARY;

			}
		}
	}

	void CFileUtils::setFileAttributes (const char* path, CFileAttributes* fAttributes)
	{
		if (path)
		{
			DWORD attr = 0;

			if(fAttributes->archive)
				attr |= FILE_ATTRIBUTE_ARCHIVE;
			if(fAttributes->compressed)
				attr |= FILE_ATTRIBUTE_COMPRESSED;
			if(fAttributes->directory)
				attr |= FILE_ATTRIBUTE_DIRECTORY;
			if(fAttributes->encripted)
				attr |= FILE_ATTRIBUTE_ENCRYPTED;
			if(fAttributes->hidden)
				attr |= FILE_ATTRIBUTE_HIDDEN;
			if(fAttributes->normal)
				attr |= FILE_ATTRIBUTE_HIDDEN;
			if(fAttributes->readOnly)
				attr |= FILE_ATTRIBUTE_NORMAL;
			if(fAttributes->system)
				attr |= FILE_ATTRIBUTE_READONLY;
			if(fAttributes->temporary)
				attr |= FILE_ATTRIBUTE_SYSTEM;

			::SetFileAttributes (path, attr);
		}
	}

	bool CFileUtils::createFile (const char* path)
	{
		bool ret = false;
		char truncatedPath[MAX_PATH_SIZE]= {'\0'};

		//extrai o diretorio do arquivo
		extractDirectory (path, truncatedPath);

		//verifica se o diretorio existe
		if (!fileExists(truncatedPath))
		{
			//se nao existe entao criamos
			createDirectoryRecursive(truncatedPath);
		}

		//agora que ja existe o diretorio vamos criar o arquvo em branco
		FILE* fp = fopen (path, "wcS");
		if (fp)
		{
			ret = true;
			fclose (fp);
		}

		return ret;
	}

	bool CFileUtils::waitUntilFileExist (const char* file, int timeout)
	{
		bool ret = false;

		int start = GetTickCount();

		while ((unsigned int)timeout == INFINITE ||
				(GetTickCount() - start) < (unsigned int)timeout)
		{
			if (CFileUtils::fileExists(file))
			{
				ret = true;
				break;
			}
			Sleep (100);
		}

		return ret;
	}

	bool CFileUtils::safeCopy (const char* sourceFile, const char* destFile)
	{
		bool ret = false;
		FILE* fpSource = fopen (sourceFile, "rb");

		if (fpSource)
		{
			//seta modo para binario
			_setmode (_fileno(fpSource), _O_BINARY);

			//abre arquivo destino
			FILE* fpDest = fopen (destFile, "wbcS");

			if (fpDest)
			{
				//seta modo para binario
				_setmode (_fileno(fpDest), _O_BINARY);

				//obtem o handle para o arquivo
				HANDLE hfile = (HANDLE) _get_osfhandle (_fileno (fpDest));


				fseek (fpSource, 0L, SEEK_END);
				unsigned int fileLen = ftell (fpSource);
				fseek (fpSource, 0L, SEEK_SET);

				unsigned char* buffer = new unsigned char[fileLen];

				memset (buffer, '\0', fileLen);

				fread (buffer, fileLen, 1, fpSource);
				fwrite (buffer, fileLen, 1, fpDest);


				delete[] buffer;

				fflush(fpDest);

				//descarrega buffer da memoria para o disco
				if (!FlushFileBuffers (hfile))
				{
					ret = false;
				}
				else
				{
					ret = true;
				}

				fclose (fpDest);
			}

			fclose (fpSource);
		}

		return ret;
	}

	bool CFileUtils::readAllFile (const char* file, CBuffer& fileContent)
	{
		bool ret = false;
		FILE* fpSource = fopen (file, "rb");

		fileContent.clear();
		if (fpSource)
		{
			fseek (fpSource, 0L, SEEK_END);
			unsigned int fileLen = ftell (fpSource);
			fseek (fpSource, 0L, SEEK_SET);

			unsigned char* buffer = new unsigned char[fileLen];
			memset (buffer, '\0', fileLen);

			for (unsigned int i = 0; i < fileLen; i++)
			{
				fread (&buffer[i], 1, 1, fpSource);
			}
			fclose (fpSource);

			fileContent.setBuffer(buffer, fileLen);

			delete[] buffer;
			ret = true;
		}

		return ret;
	}

	bool CFileUtils::readAllFile (const char* file, CBasicString& fileContent)
	{
		bool ret;
		CBuffer buff;

		fileContent.clear();
		ret = readAllFile (file, buff);

		if (ret)
		{
			fileContent.append((const char*)buff.getBuffer());
		}

		return ret;
	}

	bool CFileUtils::readAllFile (const char* file, CBasicStringList& fileContent, const char* separators)
	{
		bool ret;
		CBuffer buff;

		fileContent.clear();
		ret = readAllFile (file, buff);

		if (ret)
		{
			fileContent.deserialize((const char*)buff.getBuffer(), separators);
		}
		
		return ret;
	}

	bool CFileUtils::appendDataInFile (const char* file, const void* buffer, unsigned int len)
	{
		bool ret = false;
		FILE* fp;

		fp = fopen (file, "abcS");

		if (fp)
		{
			if (len > 0)
			{
				fwrite (buffer, len, 1, fp);
			}

			fflush(fp);
			fclose (fp);
			ret = true;
		}

		return ret;
	}

	bool CFileUtils::appendDataInFile (const char* file, CBasicStringList& data)
	{
		bool ret = false;
		FILE* fp;

		fp = fopen (file, "abcS");

		if (fp)
		{
			int listLen = data.count();

			for (int i = 0; i < listLen; i++)
			{
				fprintf(fp, "%s\n", data[i]);
			}
			fflush(fp);
			fclose (fp);
			ret = true;
		}

		return ret;
	}

	bool CFileUtils::appendFileInFile (const char* fileFrom, const char* fileTo)
	{
		bool ret = false;
		
		CBuffer content;
		if (CFileUtils::readAllFile (fileFrom, content))
		{
			ret = CFileUtils::appendDataInFile (fileTo, (const void*)content.getBuffer(), content.length());
		}
		
		return ret;
	}

	bool CFileUtils::renameFiles (CBasicStringList& from, CBasicStringList& to)
	{
		if (from.count() != to.count())
		{
			return false;
		}
		
		bool ret = true;
		int count = from.count();
		const char* pFrom;
		const char* pTo;
		
		for (int i = 0; i < count; i++)
		{
			pFrom = from[i];
			pTo = to[i];
			
			if (CFileUtils::fileExists(pFrom))
			{
				CFileUtils::deleteFile(pTo);
				if (!CFileUtils::moveFile(pFrom, pTo, true))
				{
					ret = false;
				}
			}
		}
		return ret;
	}
#else
	/* LINUX */
	bool CFileUtils::deleteFile(const char* fullPath)
	{
		bool ret = false;
		
		if(remove(fullPath) == 0)
		{
			ret = true;
		}
		
	#ifdef Mx870
		svcSystem("sync");
	#endif
		
		return ret;
	}

	bool CFileUtils::createFile(const char* fullPath)
	{
		bool ret = false;
		char truncatedPath[MAX_PATH_SIZE]= {'\0'};

		//extrai o diretorio do arquivo
		extractDirectory (fullPath, truncatedPath);

		//verifica se o diretorio existe
		if (!fileExists(truncatedPath))
		{
			//se nao existe entao criamos
			createDirectoryRecursive(truncatedPath);
		}
		
	#ifdef Mx870

		char cmd[1024] = {0};
		sprintf(cmd, "touch \"%s\"", fullPath);
		svcSystem(cmd);
		svcSystem("sync");
		ret = true;
		
	#else

		//agora que ja existe o diretorio vamos criar o arquvo em branco
		FILE* fp = fopen (fullPath, "w");
		if (fp)
		{
			ret = true;
			fclose (fp);
		}
		
	#endif

		return ret;
	}							

	bool CFileUtils::changeFilePermission(const char* fullPath, mode_t mode)
	{
		bool ret = false;

		ret = chmod(fullPath, mode) == 0;
		
		return ret;
	}

	bool CFileUtils::copyFile(const char* path, const char* destPath)
	{
		bool ret = false;
		char buffer[2048] = {0};
		
		if(!fileExists(path))
		{
			return ret;
		}
		
	#ifdef Mx870

		sprintf(buffer, "cp %s %s", path, destPath);
		svcSystem(buffer);
		svcSystem("sync");
		ret = true;
		
	#else
		int of = 0;
		int of1 = 0;
		int rf = 0;
		int wf = 0;
		
		of = open(path, O_RDONLY);
		if(of == 0)
		{
			return ret;
		}
		
		of1 = open(destPath, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);
		if(of1 == 0)
		{
			close(of);
			return ret;
		}
		
		while((rf = read(of, buffer, sizeof(buffer))) > 0)
		{
			wf = write(of1, buffer, rf);
		}

		ret = true;
		
		close(of);
		close(of1);
	#endif

		return ret;
	}

	bool CFileUtils::getFilePath(const char* fullFileName, char* filePath)
	{

		bool result = false;
		char fileName[HSTAPI_MAX_FILENAME_SIZE] = {0};
		char fileExtension[HSTAPI_MAX_FILENAME_SIZE] = {0};

		//calling method that evaluates the file path and split its parts.
		result = breakFileName(fullFileName, filePath, fileName, fileExtension);

		return(result);

	}

	bool CFileUtils::getFileName(const char* fullFileName, char* fileName)
	{

		bool result = false;
		char filePath[HSTAPI_MAX_PATH_SIZE] = {0};
		char fileExtension[HSTAPI_MAX_FILENAME_SIZE] = {0};

		//calling method that evaluates the file path and split its parts.
		if((result = breakFileName(fullFileName, filePath, fileName, fileExtension)) == true)
		{
			if (strlen(fileExtension) > 0)
			{
				strcat(fileName, ".");
				strcat(fileName, fileExtension);
			}
		}
		
		return(result);

	}

	bool CFileUtils::getFileExtension(const char* fullFileName, char* fileExtension)
	{

		bool result = false;
		char filePath[CFileUtils::MAX_PATH_SIZE] = {0};
		char fileName[CFileUtils::MAX_PATH_SIZE] = {0};

		//calling method that evaluates the file path and split its parts.
		result = breakFileName(fullFileName, filePath, fileName, fileExtension);

		return(result);

	}

	bool CFileUtils::breakFileName(const char* fullFileName, char* filePath, char* fileName, char* fileExtension)
	{

		char* token = NULL;
		char* lastToken = NULL;

		//Locate the last ocurrence of "\"
		token = (char*) strpbrk(fullFileName, "\\");
		if (token == NULL)
		{
			token = (char*) strpbrk(fullFileName, "/");
			if (token == NULL)
			{
				return false;
			}
		}

		do
		{
			lastToken = token + 1;
			token = strpbrk(token + 1, "\\");
			if (token == NULL)
			{
				token = strpbrk(lastToken, "/" );
			}
		} while(token != NULL);
		strncpy(filePath, fullFileName , (lastToken - fullFileName));

		int len = (lastToken - fullFileName);
		if (len > 0)
		{
			filePath[ len - 1 ]= '\0';
		}
		else
		{
			filePath[ 0 ]= '\0';
		}

		strcpy(fileName, lastToken);

		token = strpbrk(fileName, ".");
		if(token != NULL)
		{
			do
			{
				lastToken = token;
				token = strpbrk(token + 1, ".");
			}while(token != NULL);

			if (lastToken)
			{
				strcpy(fileExtension, lastToken + 1);
				*lastToken = '\0';
			}
			else
			{
				fileExtension[ 0 ] = '\0';
			}
		}
		
		return true;
	}

	//his method extract one level of directory of the Path provided
	///Ex : path   = "c:\test\dir1\abc"
	///     return = "c:\test\dir1"
	///It returns false if couldnot extract the dir
	bool CFileUtils::extractDirectory(const char* path, char* truncatedPath)
	{
		char* token = NULL;

		if (path == NULL || truncatedPath == NULL)
		{
			return false;
		}
		
		token = (char*) strrchr(path, '\\');
		if (token == NULL)
		{
			token = (char*) strrchr(path, '/');
		}
		
		truncatedPath[0] = '\0';
		//memset( truncatedPath, '\0', sizeof( truncatedPath ) );

		//If the ocurrence was found....
		if( token != NULL )
		{
			strncpy( truncatedPath, path , (token-path) );
			return true;
		}
		else
		{
			return false;
		}
	}

	//Create the directorty specified by "path", and all the directories needed
	///in the path, if they do not exist
	void CFileUtils::createDirectoryRecursive( const char* path )
	{
		if (path == NULL || path[0] == '\0')
		{
			return;
		}

		//Hold the path, minus the last directory
		char truncatedPath[HSTAPI_MAX_FULL_PATH_SIZE]= {'\0'};

		//Locate the first ocurrence of "\" ( should be in c:\\dir.... )
		char* token= (char*) strchr( path, '\\' );
		char* token2 = (char*) strchr( path, '/' );

		if (token == NULL && token2 == NULL)
		{
			//nao achou nenhuma das barras
			return;
		}
		else if (token == NULL && token2 != NULL)
		{
			//achou apenas a segunda
			token = token2;
		}
		else if (token != NULL && token2 != NULL)
		{
			//achou as duas pega a primeira
			if (token2 < token)
			{
				token = token2;
			}
		}

		//Verify if the directory exist�s
		///zero     = exists
		///non zero = does�t exists
		if( access( path, 0 ) != 0 ){

			//Extract the last level of the directory tree
			CFileUtils::extractDirectory( path, truncatedPath );

			//Call recursively this method, in order to create
			///the deeper directories first
			CFileUtils::createDirectoryRecursive( truncatedPath );

			//Finally create the directory
			mkdir( path, 0777 );

		};//if( access( path, 0 ) != 0 )

	}

	bool CFileUtils::fileExists (const char* path)
	{
		///zero     = exists
		///non zero = does�t exists
		return (access( path , 0 ) == 0); 
	}

	bool CFileUtils::isDirectory(const char* path)
	{
		struct stat statbuf;
		
		if(stat(path, &statbuf) < 0)
		{
			return false;
		}

		return ((statbuf.st_mode & S_IFMT) == S_IFDIR) ? true : false;
	}

	bool CFileUtils::moveFile(const char* fromPath, const char* toPath, bool overwrite)
	{
		//verifica se eh para sobrescrever
		if (overwrite)
		{
			deleteFile(toPath);
		}
		
	#ifdef Mx870
		
		bool ret = false;
		char cmd[2048] = {0};
		sprintf(cmd, "cp -f \"%s\" \"%s\"", fromPath, toPath);
		svcSystem(cmd);
		svcSystem("sync");
		
		if(CFileUtils::fileExists(toPath))
		{
                    memset(cmd, 0, sizeof(cmd));
			ret = true;
			sprintf(cmd, "rm -f \"%s\"", fromPath);
			svcSystem(cmd);
			svcSystem("sync");
		}
		
		return ret;
		
	#else

		///non zero	= Error
		///zero		= OK
		return (rename(fromPath, toPath) == 0);
		
	#endif
	}

	bool CFileUtils::safeCopy (const char* sourceFile, const char* destFile)
	{
		bool ret = false;
	   
		return ret;
	}

	bool CFileUtils::waitUntilFileExist(const char* path, int timeout)
	{
		bool ret = false;
		int count = 0;
		
		while(count <= timeout)
		{
			if(CFileUtils::fileExists(path))
			{
				ret = true;
				break;
			}
			
			CTimeUtils::msleep(100);
			count += 100;
		}
		
		return ret;
	}

	long CFileUtils::fileLength(const char* fullPath)
	{
		long length = -1;
		struct stat status;
		
		if(stat(fullPath, &status) == 0)
		{
			length = status.st_size;
		}
		
		return length;
	}

	bool CFileUtils::readAllFile (const char* file, CBuffer& fileContent)
	{
		bool ret = false;
		FILE* fpSource = fopen (file, "rb");
		
		fileContent.clear();
		if (fpSource)
		{
			fseek (fpSource, 0L, SEEK_END);
			unsigned int fileLen = ftell (fpSource);
			fseek (fpSource, 0L, SEEK_SET);

			unsigned char* buffer = new unsigned char[fileLen];
			memset (buffer, '\0', fileLen);
		
			if (fread (buffer, fileLen, 1, fpSource) == 1)
			{
				fileContent.setBuffer(buffer, fileLen);
				ret = true;
			}
			fclose (fpSource);
			delete[] buffer;
		}

		return ret;
	}
	
	bool CFileUtils::readAllFile (const char* file, CBasicString& fileContent)
	{
		bool ret;
		CBuffer buff;

		fileContent.clear();
		ret = readAllFile (file, buff);

		if (ret)
		{
			fileContent.append((const char*)buff.getBuffer());
		}

		return ret;
	}

	bool CFileUtils::readAllFile (const char* file, CBasicStringList& fileContent, const char* separators)
	{
		bool ret;
		CBuffer buff;

		fileContent.clear();
		ret = readAllFile (file, buff);

		if (ret)
		{
			fileContent.deserialize((const char*)buff.getBuffer(), separators);
		}
		
		return ret;
	}

	bool CFileUtils::directoryExists(const char* fullPath)
	{
		return fileExists(fullPath);
	}

	void CFileUtils::deleteDirectory (const char* path)
	{
		if (CFileUtils::fileExists(path) == false)
		{
			return ;
		}

		const char* pStr = NULL;
		CBasicStringList allFiles;
		CFileUtils::listFileNames (path, false, allFiles, EFOAll);

		int len = allFiles.count();

		for (int i = 0 ; i < len  ; i++)
		{
			pStr = allFiles.elmtAt(i);

			if (isDirectory(pStr))
			{
				deleteDirectory (pStr);
			}
			else
			{
				deleteFile(pStr);
			}
		}
		rmdir(path);
	}

	void CFileUtils::getFileAttributes (const char* path, CFileAttributes* fAttributes)
	{
		// TODO for Linux, if it�s possible
	}

	void CFileUtils::setFileAttributes (const char* path, CFileAttributes* fAttributes)
	{
		// TODO for Linux, if it�s possible
	}

	void CFileUtils::listFileNames ( const char* basePath,
									bool recursive, 
									CBasicStringList& filesList, 
									EFindOptions findOptions,
									bool fullPath)
	{
		CFileUtils::listFileNames ( basePath,
									"*",
									recursive, 
									filesList, 
									findOptions,
									fullPath);
	}
#ifdef WIN32
	void CFileUtils::listFileNames ( const char* basePath,
									const char* searchCriterion,
									bool recursive, 
									CBasicStringList& filesList, 
									EFindOptions findOptions,
									bool fullPath)
									
	{
		struct dirent* ent = NULL;
		DIR* dir = NULL;
		
		char path[HSTAPI_MAX_FULL_PATH_SIZE] = {0};

		sprintf (path, "%s", basePath);
		
		//carrega estrutura com informa��es do arquivo
		dir = opendir(path);

		if (dir != NULL)
		{
			
			while ((ent = readdir(dir)))
			{
				if(strncmp(ent->d_name, ".", 1) != 0)
				{
					int currPathLen = strlen (basePath);
					
					//concatena no nome do diretorio no basePath
					if (basePath[currPathLen - 1] == '/')
					{
						sprintf (path, "%s%s", basePath, ent->d_name);
					}
					else
					{
						sprintf (path, "%s/%s", basePath, ent->d_name);
					}			
				
					//verifica se � recursivo, se for, entao verificamos se � um diretorio
					if (recursive && isDirectory(path))
					{
						//se for recursivo e � um diret�rio, ainda temos que ver se n�o eh .
		
						if (findOptions == EFOAll || findOptions == EFOOnlyDirectories)
						{
							if (fullPath)
							{
								if(CWildcard::match(ent->d_name, searchCriterion, true))
								{
									filesList.add(path);
								}
							}
							else
							{
								if(CWildcard::match(ent->d_name, searchCriterion, true))
								{
									int basePathLen = strlen (basePath);
									filesList.add(path + basePathLen + 1);
								}
							}
						}

						CFileUtils::listFileNames (path, searchCriterion, true, filesList, findOptions, fullPath);
					}
					else
					{
						if (isDirectory(path))
						{
							//nao � recursivo e � diretorio
		
							//se for diretorio e for "." nao inserimos na lista
							if(strncmp(ent->d_name, ".", 1))
							{
								//nao � ".", eh nome de diretorio
								if (findOptions == EFOAll || findOptions == EFOOnlyDirectories)
								{
									sprintf(path, "%s/%s", basePath, ent->d_name);
									
									if (fullPath)
									{
										if(CWildcard::match(ent->d_name, searchCriterion, true))
										{
											filesList.add(path);
										}
									}
									else
									{
										if(CWildcard::match(ent->d_name, searchCriterion, true))
										{
											int basePathLen = strlen (basePath);
											filesList.add(path + basePathLen + 1);
										}
									}
								}
							}					
						}
						else
						{
							//n�o � recursivo e � arquivo
		
							//se for diretorio e for "." nao inserimos na lista
							if(strncmp(ent->d_name, ".", 1))
							{
								//� arquivo
								if (findOptions == EFOAll || findOptions == EFOOnlyFiles)
								{
									sprintf(path, "%s/%s", basePath, ent->d_name);
									if (fullPath)
									{
										if(CWildcard::match(ent->d_name, searchCriterion, true))
										{
											filesList.add(path);
										}
									}
									else
									{
										if(CWildcard::match(ent->d_name, searchCriterion, true))
										{
											int basePathLen = strlen (basePath);
											filesList.add(path + basePathLen + 1);
										}
									}
								}
							}
						}
					}
				}
			}
			closedir(dir);
		
			
			
			
		}
	}
#endif

	bool CFileUtils::appendDataInFile (const char* file, const void* buffer, unsigned int len)
	{
		bool ret = false;
		FILE* fp;

		fp = fopen (file, "ab");

		if (fp)
		{
			if (len > 0)
			{
				fwrite (buffer, len, 1, fp);
			}
			fclose (fp);
			ret = true;
		}

		return ret;
	}

	bool CFileUtils::appendDataInFile (const char* file, CBasicStringList& data)
	{
		bool ret = false;
		FILE* fp;

		fp = fopen (file, "ab");

		if (fp)
		{
			int listLen = data.count();

			for (int i = 0; i < listLen; i++)
			{
				fprintf(fp, "%s\n", data[i]);
			}

			fclose (fp);
			ret = true;
		}

		return ret;
	}

	bool CFileUtils::appendFileInFile (const char* fileFrom, const char* fileTo)
	{
		bool ret = false;
		
		CBuffer content;
		if (CFileUtils::readAllFile (fileFrom, content))
		{
			ret = CFileUtils::appendDataInFile (fileTo, (const void*)content.getBuffer(), content.length());
		}
		
		return ret;
	}

	bool CFileUtils::renameFiles (CBasicStringList& from, CBasicStringList& to)
	{
		if (from.count() != to.count())
		{
			return false;
		}
		
		bool ret = true;
		int count = from.count();
		const char* pFrom;
		const char* pTo;
		
		for (int i = 0; i < count; i++)
		{
			pFrom = from[i];
			pTo = to[i];
			
			if (CFileUtils::fileExists(pFrom))
			{
				CFileUtils::deleteFile(pTo);
				if (!CFileUtils::moveFile(pFrom, pTo, true))
				{
					ret = false;
				}
			}
		}
		return ret;
	}

	bool CFileUtils::isDiskPresent(const char* device)
	{
		char commandLine[32] = {0};
		sprintf(commandLine, "stat %s", device);
		int rc = system(commandLine);
		return (rc == 0);
	}

	bool CFileUtils::isDiskMounted(const char* device)
	{
		char commandLine[128] = {0};
		sprintf(commandLine, "mount | grep %s", device);
		int rc = system(commandLine);
		return (rc == 0);
	}

	bool CFileUtils::mount(const char* device, const char* mountPoint)
	{
		char commandLine[128] = {0};
		sprintf(commandLine, "mkdir -p %s", mountPoint);
		int rc = system(commandLine);
		if(rc == 0)
		{
			sprintf(commandLine, "mount %s %s", device, mountPoint);
			rc = system(commandLine);
		}
		return (rc == 0);
	}

	bool CFileUtils::umount(const char* mountPoint)
	{
		char commandLine[128] = {0};
		sprintf(commandLine, "umount %s", mountPoint);
		int rc = system(commandLine);
		return (rc == 0);
	}
#endif	

	void CFileUtils::flush()
	{
#ifdef WIN32
		fflush(NULL);
#else
#ifdef Mx870
		svcSystem("sync");
#else
		system("sync");
#endif		
#endif	
	}

