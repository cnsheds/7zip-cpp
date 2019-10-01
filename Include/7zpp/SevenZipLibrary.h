#pragma once

#include "SevenZipException.h"
#include "CompressionFormat.h"

namespace XSevenZip
{
	class SevenZipLibrary
	{
	private:

		typedef UINT32 (WINAPI * CreateObjectFunc)( const GUID* clsID, const GUID* interfaceID, void** outObject );

		HMODULE				m_dll;
		CreateObjectFunc	m_func;
		TString				m_lastError;

	public:

		SevenZipLibrary();
		~SevenZipLibrary();

		bool Load();
		bool Load( const TString& libraryPath );
		void Free();
		TString GetError();

		bool CreateObject( const GUID& clsID, const GUID& interfaceID, void** outObject ) const;
	};
}
