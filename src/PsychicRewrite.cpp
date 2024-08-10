#include "PsychicRewrite.h"
#include "PsychicRequest.h"

    PsychicRewrite::PsychicRewrite(const char* from, const char* to):
		_fromPath(from),
		_toUri(to),
		_toPath(String()),
		_toParams(String()),
		_filter(nullptr)
	{
      int index = _toUri.indexOf('?');
      if (index > 0) {
        _toParams = _toUri.substring(index + 1);
        _toPath = _toUri.substring(0, index);
      }
	  else
	  	_toPath = _toUri;
    }
    PsychicRewrite::~PsychicRewrite()
	{
		
	}

	PsychicRewrite* PsychicRewrite::setFilter(PsychicFilterFunction fn)
	{
		_filter = fn; return this;
	}

	PsychicRewrite* PsychicRewrite::setFilter(PsychicRequestFilterFunction fn)
	{
		_requestFilter = fn; return this;
	}
    
	bool PsychicRewrite::filter(PsychicRequest *request) const
	{
		bool ret = _filter == NULL || _filter();
		if(!ret)
			return false;
		if(_requestFilter == NULL)
			return true;
		request->loadParams();
		return _requestFilter(request);
	}
    
	const String& PsychicRewrite::from(void) const
	{
		return _fromPath;
	}
    const String& PsychicRewrite::toUrl(void) const
	{
		return _toUri;
	}
    
	const String& PsychicRewrite::params(void) const
	{
		return _toParams;
	}

    bool PsychicRewrite::match(PsychicRequest *request)
	{
		if (!filter(request))
			return false;

		return _fromPath == request->path();
	}