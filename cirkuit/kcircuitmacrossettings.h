// This file is generated by kconfig_compiler from kcircuitmacros.kcfg.
// All changes you do to this file will be lost.
#ifndef KCIRCUITMACROSSETTINGS_H
#define KCIRCUITMACROSSETTINGS_H

#include <kconfigskeleton.h>
#include <kdebug.h>

class KCircuitMacrosSettings : public KConfigSkeleton
{
  public:

    static KCircuitMacrosSettings *self();
    ~KCircuitMacrosSettings();

    /**
      Set Refresh interval.
    */
    static
    void setRefreshInterval( int v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "refreshInterval" ) ))
        self()->mRefreshInterval = v;
    }

    /**
      Get Refresh interval.
    */
    static
    int refreshInterval()
    {
      return self()->mRefreshInterval;
    }

    /**
      Set The path where the manual is located.
    */
    static
    void setManualPath( const QString & v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "manualPath" ) ))
        self()->mManualPath = v;
    }

    /**
      Get The path where the manual is located.
    */
    static
    QString manualPath()
    {
      return self()->mManualPath;
    }

    /**
      Set The path where the examples are located.
    */
    static
    void setExamplesPath( const QString & v )
    {
      if (!self()->isImmutable( QString::fromLatin1 ( "examplesPath" ) ))
        self()->mExamplesPath = v;
    }

    /**
      Get The path where the examples are located.
    */
    static
    QString examplesPath()
    {
      return self()->mExamplesPath;
    }

  protected:
    KCircuitMacrosSettings();
    friend class KCircuitMacrosSettingsHelper;


    // general
    int mRefreshInterval;
    QString mManualPath;
    QString mExamplesPath;

  private:
};

#endif

