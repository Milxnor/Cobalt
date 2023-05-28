#pragma once

enum class ECobaltUsage
{
	Private,
	Hybrid,
	// RecordingRequests // todo?
};

#define URL_PROTOCOL "http"
#define URL_HOST "127.0.0.1"
#define URL_PORT "3551"

#define SHOW_WINDOWS_CONSOLE

constexpr static ECobaltUsage CobaltUsage = ECobaltUsage::Private;