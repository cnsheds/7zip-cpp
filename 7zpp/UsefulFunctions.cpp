#include "stdafx.h"
#include "UsefulFunctions.h"
#include <vector>
#include "PropVariant.h"


namespace XSevenZip
{
	using namespace intl;

	const GUID* UsefulFunctions::GetCompressionGUID(const CompressionFormatEnum& format)
	{
		const GUID* guid = NULL;

		switch (format)
		{
		case CompressionFormat::Zip:
			guid = &XSevenZip::intl::CLSID_CFormatZip;
			break;

		case CompressionFormat::GZip:
			guid = &XSevenZip::intl::CLSID_CFormatGZip;
			break;

		case CompressionFormat::BZip2:
			guid = &XSevenZip::intl::CLSID_CFormatBZip2;
			break;

		case CompressionFormat::Rar:
			guid = &XSevenZip::intl::CLSID_CFormatRar;
			break;

		case CompressionFormat::Rar5:
			guid = &XSevenZip::intl::CLSID_CFormatRar5;
			break;

		case CompressionFormat::Tar:
			guid = &XSevenZip::intl::CLSID_CFormatTar;
			break;

		case CompressionFormat::Iso:
			guid = &XSevenZip::intl::CLSID_CFormatIso;
			break;

		case CompressionFormat::Cab:
			guid = &XSevenZip::intl::CLSID_CFormatCab;
			break;

		case CompressionFormat::Lzma:
			guid = &XSevenZip::intl::CLSID_CFormatLzma;
			break;

		case CompressionFormat::Lzma86:
			guid = &XSevenZip::intl::CLSID_CFormatLzma86;
			break;

		case CompressionFormat::Dmg:
			guid = &XSevenZip::intl::CLSID_CFormatDmg;
			break;

		case CompressionFormat::Nsis:
			guid = &XSevenZip::intl::CLSID_CFormatNsis;
			break;

		case CompressionFormat::Xz:
			guid = &XSevenZip::intl::CLSID_CFormatXz;
			break;

		default:
			guid = &XSevenZip::intl::CLSID_CFormat7z;
			break;
		}
		return guid;
	}

	CComPtr< IInArchive > UsefulFunctions::GetArchiveReader(const SevenZipLibrary& library, const CompressionFormatEnum& format)
	{
		const GUID* guid = GetCompressionGUID(format);

		CComPtr< IInArchive > archive;
		library.CreateObject(*guid, IID_IInArchive, reinterpret_cast<void**>(&archive));
		return archive;
	}

	CComPtr< IOutArchive > UsefulFunctions::GetArchiveWriter(const SevenZipLibrary& library, const CompressionFormatEnum& format)
	{
		const GUID* guid = GetCompressionGUID(format);

		CComPtr< IOutArchive > archive;
		library.CreateObject(*guid, IID_IOutArchive, reinterpret_cast< void** >(&archive));
		return archive;
	}

	bool UsefulFunctions::GetNumberOfItems(const SevenZipLibrary & library, const TString & archivePath,
		CompressionFormatEnum &format, size_t & numberofitems)
	{
		CComPtr< IStream > fileStream = FileSys::OpenFileToRead(archivePath);

		if (fileStream == NULL)
		{
			return false;
			//throw SevenZipException( StrFmt( _T( "Could not open archive \"%s\"" ), m_archivePath.c_str() ) );
		}

		CComPtr< IInArchive > archive = UsefulFunctions::GetArchiveReader(library, format);
		CComPtr< InStreamWrapper > inFile = new InStreamWrapper(fileStream);
		CComPtr< ArchiveOpenCallback > openCallback = new ArchiveOpenCallback();

		HRESULT hr = archive->Open(inFile, 0, openCallback);
		if (hr != S_OK)
		{
			return false;
			//throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
		}

		UInt32 mynumofitems;
		hr = archive->GetNumberOfItems(&mynumofitems);
		if (hr != S_OK)
		{
			return false;
			//throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
		}
		numberofitems = size_t(mynumofitems);

		archive->Close();
		return true;
	}

	bool UsefulFunctions::GetItemsNames(const SevenZipLibrary & library, const TString & archivePath,
		CompressionFormatEnum &format, size_t & numberofitems, 
		std::vector<TString> & itemnames, std::vector<size_t> & origsizes)
	{
		CComPtr< IStream > fileStream = FileSys::OpenFileToRead(archivePath);

		if (fileStream == NULL)
		{
			return false;
			//throw SevenZipException( StrFmt( _T( "Could not open archive \"%s\"" ), m_archivePath.c_str() ) );
		}

		CComPtr< IInArchive > archive = UsefulFunctions::GetArchiveReader(library, format);
		CComPtr< InStreamWrapper > inFile = new InStreamWrapper(fileStream);
		CComPtr< ArchiveOpenCallback > openCallback = new ArchiveOpenCallback();

		HRESULT hr = archive->Open(inFile, 0, openCallback);
		if (hr != S_OK)
		{
			return false;
			//throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
		}

		UInt32 mynumofitems;
		hr = archive->GetNumberOfItems(&mynumofitems);
		if (hr != S_OK)
		{
			return false;
			//throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
		}
		numberofitems = size_t(mynumofitems);

		itemnames.clear();
		itemnames.resize(numberofitems);

		origsizes.clear();
		origsizes.resize(numberofitems);

		for (UInt32 i = 0; i < numberofitems; i++)
		{
			{
				// Get uncompressed size of file
				CPropVariant prop;
				hr = archive->GetProperty(i, kpidSize, &prop);
				if (hr != S_OK)
				{
					return false;
					//throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
				}

				int size = prop.intVal;
				origsizes[i] = size_t(size);

				// Get name of file
				hr = archive->GetProperty(i, kpidPath, &prop);
				if (hr != S_OK)
				{
					return false;
					//throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
				}

				//valid string? pass back the found value and call the callback function if set
				if (prop.vt == VT_BSTR) {
					WCHAR* path = prop.bstrVal;
					std::wstring mypath(path);
					itemnames[i] = mypath;
				}
			}
		}

		archive->Close();
		return true;
	}

	bool UsefulFunctions::DetectCompressionFormat(const SevenZipLibrary & library, const TString & archivePath, 
		CompressionFormatEnum & archiveCompressionFormat)
	{
		std::vector<CompressionFormatEnum> myAvailableFormats;

		// Add more formats here if 7zip supports more formats in the future
		myAvailableFormats.push_back(CompressionFormat::Zip);
		myAvailableFormats.push_back(CompressionFormat::SevenZip);
		myAvailableFormats.push_back(CompressionFormat::Rar);
		myAvailableFormats.push_back(CompressionFormat::Rar5);
		myAvailableFormats.push_back(CompressionFormat::GZip);
		myAvailableFormats.push_back(CompressionFormat::BZip2);
		myAvailableFormats.push_back(CompressionFormat::Tar);
		myAvailableFormats.push_back(CompressionFormat::Lzma);
		myAvailableFormats.push_back(CompressionFormat::Lzma86);
		myAvailableFormats.push_back(CompressionFormat::Cab);
		myAvailableFormats.push_back(CompressionFormat::Iso);
		myAvailableFormats.push_back(CompressionFormat::Dmg);
		myAvailableFormats.push_back(CompressionFormat::Nsis);
		myAvailableFormats.push_back(CompressionFormat::Xz);

		// Check each format for one that works
		for (int i = 0; i < myAvailableFormats.size(); i++)
		{
			archiveCompressionFormat = myAvailableFormats[i];

			CComPtr< IStream > fileStream = FileSys::OpenFileToRead(archivePath);
			if (fileStream == NULL)
			{
				return false;
			}

			CComPtr< IInArchive > archive = UsefulFunctions::GetArchiveReader(library, archiveCompressionFormat);
			CComPtr< InStreamWrapper > inFile = new InStreamWrapper(fileStream);
			CComPtr< ArchiveOpenCallback > openCallback = new ArchiveOpenCallback();

			HRESULT hr = archive->Open(inFile, 0, openCallback);
			if (hr == S_OK)
			{
				// We know the format if we get here, so return
				archive->Close();
				return true;
				//throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
			}

			archive->Close();
		}

		//
		//  There is a problem that GZip files will not be detected using the above method.
		//  This is a fix.
		//
		if (true)
		{
			size_t myNumOfItems = 0;
			archiveCompressionFormat = CompressionFormat::GZip;
			bool result = GetNumberOfItems(library, archivePath, archiveCompressionFormat, myNumOfItems);
			if (result == true && myNumOfItems > 0)
			{
				// We know this file is a GZip file, so return
				return true;
			}
		}

		// If you get here, the format is unknown
		archiveCompressionFormat = CompressionFormat::Unknown;
		return true;
	}

	const TString UsefulFunctions::EndingFromCompressionFormat(const CompressionFormatEnum& format)
	{
		switch (format)
		{
		case CompressionFormat::Zip:
			return _T(".zip");
			break;
		case CompressionFormat::SevenZip:
			return _T(".7z");
			break;
		case CompressionFormat::Rar:
		case CompressionFormat::Rar5:
			return _T(".rar");
			break;
		case CompressionFormat::GZip:
			return _T(".gz");
			break;
		case CompressionFormat::BZip2:
			return _T(".bz");
			break;
		case CompressionFormat::Tar:
			return _T(".tar");
			break;
		case CompressionFormat::Lzma:
			return _T(".lzma");
			break;
		case CompressionFormat::Lzma86:
			return _T(".lzma86");
			break;
		case CompressionFormat::Cab:
			return _T(".cab");
			break;
		case CompressionFormat::Iso:
			return _T(".iso");
			break;
		case CompressionFormat::Dmg:
			return _T(".dmg");
			break;
		case CompressionFormat::Xz:
			return _T(".xz");
			break;
		}
		return _T(".zip");
	}

}

