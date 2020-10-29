#include "pch.h"
#include "CHuaShiReader.h"

CHuaShiReader::CHuaShiReader()
{
	m_strFilePath.Empty();
	GetModuleFileName(nullptr, m_strFilePath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	m_strFilePath.ReleaseBuffer();
	const auto n_pos = m_strFilePath.ReverseFind('\\');
	m_strFilePath = m_strFilePath.Left(n_pos + 1);
}

CHuaShiReader::~CHuaShiReader()
{
}
