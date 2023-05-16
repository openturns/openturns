//                                               -*- C++ -*-
/**
 *  @brief Log records all user information to a file or tty
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <errno.h>

#include "openturns/Log.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/MutexLock.hxx"

BEGIN_NAMESPACE_OPENTURNS

static std::mutex Log_InstanceMutex_;
static Log * Log_P_instance_ = 0;
static const Log_init static_initializer_Log;

static inline
_Prefix make_prefix( const _Prefix::Value & color, const _Prefix::Value & nocolor, const _Prefix::Value & prefix)
{
  return _Prefix( color, nocolor, prefix );
}

std::ostream & operator << ( std::ostream & os, const _Prefix & pfx )
{
  return os << (TTY::ColoredOutput() ? pfx.color_ : pfx.nocolor_) << pfx.prefix_;
}


Log_init::Log_init()
{
  static std::once_flag flag;
  std::call_once(flag, [&]()
  {
    Log_P_instance_ = new Log;
    Log_P_instance_->push(Log::Entry(Log::INFO, "*** Log Beginning ***"));
  });
  assert(Log_P_instance_);
}

Log_init::~Log_init()
{
  static std::once_flag flag;
  std::call_once(flag, [&]()
  {
    Log_P_instance_->push(Log::Entry(Log::INFO, "*** Log End ***"));
    delete Log_P_instance_;
    Log_P_instance_ = 0;
  });
}


const Log::Severity Log::NONE    =  0;
const Log::Severity Log::ALL     = ~0;

const Log::Severity Log::DBG     = 1 << 0;
const Log::Severity Log::INFO    = 1 << 2;
const Log::Severity Log::USER    = 1 << 3;
const Log::Severity Log::WARN    = 1 << 4;
const Log::Severity Log::ERROR   = 1 << 5;
const Log::Severity Log::TRACE   = 1 << 6;

const Log::Severity Log::DEFAULT = Log::USER | Log::WARN | Log::ERROR | Log::TRACE;

static AtomicInt Log_Severity_ = Log::DEFAULT;


/* Constructor */
Log::Log()
  : logName_(),
    openturnsLogSeverityVariableName_("OPENTURNS_LOG_SEVERITY"),
    p_file_(0),
    previousMessage_(),
    count_(0),
    repeat_(1)
{
  logName_[NONE]    = make_prefix( String(TTY::GetColor(TTY::DEFAULT)), "", "   " );
  logName_[ALL]     = make_prefix( String(TTY::GetColor(TTY::DEFAULT)), "", "ALL" );
  logName_[DBG]     = make_prefix( String(TTY::GetColor(TTY::DEFAULT)), "", "DBG" );
  logName_[INFO]    = make_prefix( String(TTY::GetColor(TTY::GREENFG)), "", "INF" );
  logName_[USER]    = make_prefix( String(TTY::GetColor(TTY::CYANFG)), "", "USR" );
  logName_[WARN]    = make_prefix( String(TTY::GetColor(TTY::BLUEFG)) + String(TTY::GetColor(TTY::BOLD)), "", "WRN" );
  logName_[ERROR]   = make_prefix( String(TTY::GetColor(TTY::REDFG))  + String(TTY::GetColor(TTY::BOLD)), "", "ERR" );
  logName_[TRACE]   = make_prefix( String(TTY::GetColor(TTY::YELLOWFG)), "", "TRA" );

  initSeverityFromEnvironment();

}


/* Destructor */
Log::~Log()
{
  delete p_file_;
  p_file_ = 0;
}


/* Set Severity according to Openturns LogSeverity Variable */
void Log::initSeverityFromEnvironment()
{
  const char * logSeverityVariableContent = getenv(openturnsLogSeverityVariableName_);
  if (logSeverityVariableContent != NULL)
  {
    String severityVariableContent(logSeverityVariableContent);
    Severity theSeverity = Log::NONE;

    const char delim = ',';
    SignedInteger begPos = 0, endPos;
    do
    {
      // search token
      endPos = severityVariableContent.find(delim, begPos);
      if (endPos == static_cast<SignedInteger>(String::npos))
        endPos = severityVariableContent.size();
      const String token(severityVariableContent.substr(begPos, endPos - begPos));

      // add severity
      std::map<Severity, _Prefix >::const_iterator iter;
      for (iter = logName_.begin(); iter != logName_.end(); ++iter)
        if (token == iter->second.prefix_)
          theSeverity |= iter->first;

      // next token
      begPos = endPos + 1;
    }
    while (endPos != static_cast<SignedInteger>(severityVariableContent.size()));

    Show( theSeverity );
  }
}


template<>
MutexLockSingleton<Log>::MutexLockSingleton(Log & singleton)
  : singleton_(singleton)
  , lock_(Log_InstanceMutex_) {}


/* GetInstance gives a locked access to the singleton */
MutexLockSingleton<Log> Log::GetInstance()
{
  static const Log_init force_instantiation;
  // Log_InstanceMutex_ is always initialized
  return *Log_P_instance_;
}

void Log::Reset()
{
}


/* Log messages according to its relative severity */
void Log::Debug(const String & msg)
{
  GetInstance().lock().push(Entry(DBG, msg));
}


/*  Log messages according to its relative severity */
void Log::Info(const String & msg)
{
  GetInstance().lock().push(Entry(INFO, msg));
}



/*  Log messages according to its relative severity */
void Log::User(const String & msg)
{
  GetInstance().lock().push(Entry(USER, msg));
}



/* Log messages according to its relative severity */
void Log::Warn(const String & msg)
{
  GetInstance().lock().push(Entry(WARN, msg));
}



/* Log messages according to its relative severity */
void Log::Error(const String & msg)
{
  GetInstance().lock().push(Entry(ERROR, msg));
}


/* Log messages according to its relative severity */
void Log::Trace(const String & msg)
{
  GetInstance().lock().push(Entry(TRACE, msg));
}


/* Get/Set the severity flags for the messages logged to the file */
void Log::Show(Severity flags)
{
  Log_Severity_ = 0;
  Log_Severity_.fetchOr(flags);
}

Log::Severity Log::Flags()
{
  return Log_Severity_.fetchOr(0x00);
}

/* Flush pending messages */
void Log::Flush()
{
  GetInstance().lock().flush();
}


/* Does Log show repeated messages or not
 *  If repeat is false then Log shows every messages it receives
 *  even if they are identical to the previous ones.
 *  If repeat is true then Log only shows the first message
 *  and a message counting how much identical messages were
 *  received after that.
 */
void Log::Repeat(Bool r)
{
  GetInstance().lock().repeat(r);
}

void Log::repeat(Bool r)
{
  repeat_ = r ? 1 : 0;
}

void Log::flush()
{
  printRepeatedMessage( previousMessage_ );
  previousMessage_ = Entry();
  count_ = 0;
}


/* Append an entry at the end of the list */
void Log::push(const Entry & entry)
{
  std::ostream & os = p_file_ ? *p_file_ : std::clog;
  if (entry.sev_ & Log::Flags())
  {
    if (entry.sev_ != Log::TRACE && repeat_.get() && entry == previousMessage_) ++count_;
    else
    {
      printRepeatedMessage( previousMessage_ );
      previousMessage_ = entry ;
      count_ = 0;
      os << logName_[entry.sev_] << " - " << entry.msg_ << TTY::GetColor(TTY::DEFAULT) << std::endl;
    }
  }
}

void Log::printRepeatedMessage(const Entry & entry)
{
  std::ostream & os = p_file_ ? *p_file_ : std::clog;
  if (count_ > 0)
    os << logName_[entry.sev_] << " - (previous message repeated " << count_ << " time" << ((count_ == 1) ? "" : "s") << ")" << TTY::GetColor(TTY::DEFAULT) << std::endl;
}

/* Set the name of the log file */
void Log::SetFile(const FileName & file)
{
  GetInstance().lock().setFile(file);
}


/* Set the name of the log file */
void Log::setFile(const FileName & file)
{
  push(Entry(INFO, String("Diverting log to file: ") + file));
  push(Entry(INFO, "*** Log End ***"));
  delete p_file_;
  TTY::ShowColors( false );
  p_file_ = new std::ofstream(file.c_str());

  push(Entry(INFO, "*** Log Beginning ***"));
}

/* Color accessor */
void Log::SetColor(const Log::Severity severity,
                   const TTY::Color color)
{
  GetInstance().lock().setColor(severity, String(TTY::GetColor(color)));
}

void Log::SetColor(const Log::Severity severity,
                   const String & color)
{
  GetInstance().lock().setColor(severity, color);
}


void Log::setColor(const Log::Severity severity,
                   const String & color)
{
  if (logName_.find(severity) != logName_.end()) logName_[severity].color_ = color;
}

/* Get the color */
String Log::GetColor(const Log::Severity severity)
{
  return GetInstance().lock().getColor(severity);
}

String Log::getColor(const Log::Severity severity) const
{
  if (logName_.find(severity) != logName_.end()) return logName_[severity].color_;
  return String("");
}

END_NAMESPACE_OPENTURNS
