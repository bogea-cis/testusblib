#ifndef HST_FILEUTILS_CFILEUTILS_H
#define HST_FILEUTILS_CFILEUTILS_H

#ifdef WIN32
	#include <windows.h>
#else
	#include <sys/stat.h>
#endif
#include "CBasicStringList.h"
#include "CBasicString.h"
#include "CBuffer.h"


#ifndef WIN32
	#define HSTAPI_MAX_PATH_SIZE 			128
	#define HSTAPI_MAX_FILENAME_SIZE 		64
	#define HSTAPI_MAX_FILE_EXTENTION_SIZE 	16
	#define HSTAPI_MAX_FULL_PATH_SIZE 		HSTAPI_MAX_PATH_SIZE + HSTAPI_MAX_FILENAME_SIZE + HSTAPI_MAX_FILE_EXTENTION_SIZE
#endif

/**
\struct
\brief
*/
struct CFileAttributes
{
	bool archive;
	bool directory;
	bool compressed;
	bool encripted;
	bool hidden;
	bool normal;
	bool readOnly;
	bool system;
	bool temporary;
};

enum EFindOptions
{
	EFOAll,
	EFOOnlyFiles,
	EFOOnlyDirectories
};

/**
\class CConversion
\brief Provê métodos utilitários para tratamento de arquivo e diretórios.
Esta classe provê um conjunto de métodos estáticos para o tratamento de arquivos e diretórios.
Todos os métodos são "stateless", ou seja, não guardam informação da execução anterior.

\author Diego Felipe Lassa / Luis Gustavo
\version 1.0.0.0
\date 14/08/08
\bug
\warning
*/
class CFileUtils
{
	private:

		//!Construtor privado. A classe não deve ser instanciada.
		CFileUtils();
		virtual ~CFileUtils();
		
	public:
		
		static void flush();
		
		//!Constante que determina o numero máximo de caracteres que um path pode conter.
#ifdef WIN32
		static const int MAX_PATH_SIZE;
#else
		static const int MAX_PATH_SIZE = 260;
#endif

		/**
		\author Diego
		\brief Remove a porção do nome do arquivo de um path.
		Este método irá remover o nome do arquivo do path passado em fullFileName, retornando
		a estrutura restante em filePath.
		\param fullFileName O caminho para o arquivo. O path utilizado deve ser absoluto e conter o nome do arquivo.
		\param filePath Irá armazenar o caminho para o arquivo, excluído o nome do mesmo.
		\return True se a operação foi realizada com sucesso, FALSE se contrário.
		*/
		static bool getFilePath(const char* fullFileName, char* filePath);

		/**
		\author Diego
		\brief Returns the file name of the current file path.
		\param fullFileName O caminho para o arquivo. O path utilizado deve ser absoluto e conter o nome do arquivo.
		\param fileName Irá armazenar o nome do arquivo, extraído do path fornecido.
		\return True se a operação foi realizada com sucesso, FALSE se contrário.
		*/
		static bool getFileName(const char* fullFileName, char* fileName);

		/**
		\author Diego
		\brief Returns the file extension of the current file path.
		\param fullFileName O caminho para o arquivo. O path utilizado deve ser absoluto e conter o nome do arquivo.
		\param fileExtension Irá armazenar a extensão do arquivo, extraído do path fornecido ( ponto incluído ).
		\return True se a operação foi realizada com sucesso, FALSE se contrário.
		*/
		static bool getFileExtension(const char* fullFileName, char* fileExtension);

		/**
		\author Diego
		\brief Decompõe o path fornecido em suas partes.		
		O método irá decompor o path fornecido em fullFilePath em seus membros :
		-#filePath O caminho para o arquivo, excluindo o nome do arquivo.
		-#fileName O nome do arquivo.
		-#fileExtension A extensão do arquivo ( incluído o ponto ).
		\param fullFileName O caminho para o arquivo. O path utilizado deve ser absoluto e conter o nome do arquivo.
		\param filePath Irá armazenar o caminho para o arquivo, excluído o nome do mesmo.
		\param fileName Irá armazenar o nome do arquivo, extraído do path fornecido.
		\param fileExtension Irá armazenar a extensão do arquivo, extraído do path fornecido ( ponto incluído ).
		\return True se a operação foi realizada com sucesso, FALSE se contrário.
		*/
		static bool breakFileName(const char* fullFileName, char* filePath, char* fileName, char* fileExtension);

		/**Extrai um nível de diretório do path passado em path
		\author Diego
		Ex : path   = "c:\test\dir1\abc"
			 return = "c:\test\dir1"
		\return True se a operação foi realizada com sucesso, FALSE se contrário.
		*/
		static bool extractDirectory(const char* path, char* truncatedPath);

		/**
		\author Diego
		\brief Cria recursivamente a estrutura de diretórios especificado por path.
		Cria o diretório especificado por path, e todos os diretórios necessários no path, caso
		não existam.
		*/
		static void createDirectoryRecursive (const char* path);
		
		/**
		\author Luis Gustavo
		\brief Verifica se o arquivo ou diretorio especificado existe
		\param path Armazena o path do arquivo ou diretorio que será verificado
		\return true se o arquivo ou diretorio existir, false se não existir
		*/
		static bool fileExists (const char* path);

		/**
		\author Luis Gustavo
		\brief Verifica se o conteudo do path passado é um diretório valido
		\param path Armazena o path que será verificado
		\return true se for um diretório valido, false se não for
		*/
		static bool isDirectory (const char* path);

#ifdef WIN32
		/**
		\author Luis Gustavo
		\brief Verifica se a estrutura passada representa um diretorio
		\param findFileData Armazena a representação de um arquivo ou diretório
		\return true se for um diretório valido, false se não for
		*/
		static bool isDirectory (WIN32_FIND_DATA* findFileData);
#endif

		/**
		\author Luis Gustavo
		\brief Apaga arquivo especificado
		\param path Armazena o path do arquivo que será apagado
		\return true se o arquivo for apagado, false se não foi possivel apagar ou se não existia
		*/
		static bool deleteFile (const char* path);


		/**
		\author Luis Gustavo
		\brief Apaga todos os arquivos e pastas a partir do path especificado
		\param path Armazena o path do diretorio base
		*/
		static void deleteDirectory (const char* path);

		/**
		\author Luis Gustavo
		\brief Copia um arquivo de um determinado path para outro
		\param path Armazena o path do arquivo que será copiado
		\param toPath Armazena o path do arquivo que será a cópia
		\return true se o arquivo foi copiado corretamente, false se não foi possivel
		*/
		static bool copyFile (const char* path, const char* toPath);


		/**
		\author Luis Gustavo
		\brief Copia um arquivo de um determinado path para outro de forma segura
		\(sem corromper o arquivo caso haja um deligamento de equipamento)
		\param path Armazena o path do arquivo que será copiado
		\param toPath Armazena o path do arquivo que será a cópia
		\return true se o arquivo foi copiado corretamente, false se não foi possivel
		*/
		static bool safeCopy (const char* sourceFile, const char* destFile);

		/**
		\author Luis Gustavo
		\brief Move um determinado aquivo de um lugar a outro
		\param fromPath Armazena o path inicial do arquivo que se deseja mover
		\param toPath Armazena o path final do arquivo que se deseja mover
		\param overwrite determina se deve sobrescrever se existir um arquivo com o nome igual
		\return true se o arquivo foi movido corretamente, false se não foi possivel move-lo
		*/
		static bool moveFile (const char* fromPath, const char* toPath, bool overwrite);

		/**
		\author Luis Gustavo
		\brief retorna o tamanho em bytes de um arquivo especificado
		\param path Armazena o path do arquivo que será verificado
		\return tamanho do arquivo em bytes
		*/
		static long fileLength(const char* fullPath);
		
		/**
		\author Luis Gustavo
		\brief Retorna uma lista com todos os arquivos ou diretorios que satisfazem o criterio de pesquisa
		\param basePath Determina o path base onde a pesquisa será feita
		\param searchCriterion Determina o criterio da pesquisa ex a*.txt vai retornar 
		todos os arquivos txt que comecem com "a"
		\param recursive determina se a pesquisa vai incluir os sub-diretorios
		\param filesList parametro de saida que conterá o path de todos os arquivos encontrados, obs: este já deve vir alocado
		\param findOptions determina se o resultado contera somente arquivos, somente pastas ou ambos, p default é ambos
		*/
		static void listFileNames ( const char* basePath, const char* searchCriterion,  bool recursive, CBasicStringList& filesList, EFindOptions findOptions = EFOAll, bool fullPath = true);
		static void listFileNames ( const char* basePath, bool recursive, CBasicStringList& filesList, EFindOptions findOptions = EFOAll, bool fullPath = true);
		

		/**
		\author Luis Gustavo
		\brief retorna o diretorio atual
		\param path Retorna o path do diretorio atual
		\param pathLength tamanho da variavel path
		\return Ponteiro para o path
		*/
		static char* getCurrentDirectory (char* path, int pathLength);

		/**
		\author Luis Gustavo
		\brief retorna o tamanho em bytes de um arquivo especificado
		\param path Armazena o path do arquivo que será verificado
		\param fAttributes Retorna estrutura com attributos do arquivo
		*/
		static void getFileAttributes (const char* path, CFileAttributes* fAttributes);

		/**
		\author Luis Gustavo
		\brief retorna o tamanho em bytes de um arquivo especificado
		\param path Armazena o path do arquivo que será verificado
		\param fAttributes Estrutura com attributos que serao setados no arquivo
		*/
		static void setFileAttributes (const char* path, CFileAttributes* fAttributes);

		/**
		\author Luis Gustavo
		\brief Cria um arquivo em branco em um path especificado, cria o path se naop existir
		\param path Armazena o path do arquivo
		\return true se o arquivo foi criado, false se não
		*/
		static bool createFile (const char* path);


		/**
		\author Luis Gustavo
		\brief Espera um arquivo ser criado
		\param file path do arquivo
		\param timeout tempo em milisegundos
		\return true se o arquivo foi criado, false se não
		*/
		static bool waitUntilFileExist (const char* file, int timeout);


		/**
		\author Luis Gustavo
		\brief le todo o conteudo de um arquivo e retorna o buffer lido
		\param file path do arquivo
		\param fileContent conteudo do arquivo
		\return true se o conteudo foi copiado com sucesso, false se algum erro ocorreu
		*/
		static bool readAllFile (const char* file, CBuffer& fileContent);


		/**
		\author Luis Gustavo
		\brief le todo o conteudo de um arquivo e retorna o buffer lido
		\param file path do arquivo
		\param fileContent conteudo do arquivo
		\return true se o conteudo foi copiado com sucesso, false se algum erro ocorreu
		*/
		static bool readAllFile (const char* file, CBasicString& fileContent);

		
		/**
		\author Luis Gustavo
		\brief le todo o conteudo de um arquivo e retorna o buffer lido
		\param file path do arquivo
		\param fileContent conteudo do arquivo
		\return true se o conteudo foi copiado com sucesso, false se algum erro ocorreu
		*/
		static bool readAllFile (const char* file, CBasicStringList& fileContent, const char* separators);

		/**
		\author Luis Gustavo
		\brief Adiciona um determinado buffer em um arquivo
		\param file path do arquivo
		\param buffer conteudo que se deseja adicionar
		\param len tamanho do buffer
		\return true se o conteudo foi adicionado com sucesso, false se algum erro ocorreu
		*/

		static bool appendDataInFile (const char* file, const void* buffer, unsigned int len);


		/**
		\author Luis Gustavo
		\brief Adiciona uma lista de strings pulando linha a cada elemento
		\param file path do arquivo
		\param data lista de string
		\return true se o conteudo foi adicionado com sucesso, false se algum erro ocorreu
		*/
		static bool appendDataInFile (const char* file, CBasicStringList& data);
		static bool appendFileInFile (const char* fileFrom, const char* fileTo);
		static bool renameFiles (CBasicStringList& from, CBasicStringList& to);
		
#ifndef WIN32
		static bool changeFilePermission(const char* fullPath, mode_t mode);
		static bool directoryExists(const char* fullPath);
		static bool isDiskPresent(const char* device);
		static bool isDiskMounted(const char* device);
		static bool mount(const char* device, const char* mountPoint);
		static bool umount(const char* mountPoint);
#endif

};//class CFileUtils

#endif//#ifndef HST_FILEUTILS_CFILEUTILS_H
