#include "../generic - inc/hedSteamAchievements.h"

CSteamAchievements::CSteamAchievements(Achievement_t *Achievements, int NumAchievements): 				
 m_iAppID( 0 ),
 m_bInitialized( false ),
 m_CallbackUserStatsReceived( this, &CSteamAchievements::OnUserStatsReceived ),
 m_CallbackUserStatsStored( this, &CSteamAchievements::OnUserStatsStored ),
 m_CallbackAchievementStored( this, &CSteamAchievements::OnAchievementStored )
{
     m_iAppID = SteamUtils()->GetAppID();
     m_pAchievements = Achievements;
     m_iNumAchievements = NumAchievements;
     RequestStats();
}

//RequestStats()
//Parameters - None
//Returns - a bool representing if the call succeeded or not. If the call failed then most likely Steam is not initialized. Make sure you have a steam client open when you try to make this call and that SteamAPI_Init() has been called before it.
//What it does - This method basically wraps a call to SteamUserStats()->RequestCurrentStats() that is an asynchronous call to steam requesting the stats of the current user. This call needs to be made before you can set and stats or achievements. The initial call to this method is made in the constructor. You can call it again any time after that if you want to check on updated stats or achievements.


bool CSteamAchievements::RequestStats()
{
	// Is Steam loaded? If not we can't get stats.
	if(NULL == SteamUserStats() || NULL == SteamUser())
	{
		return false;
	}

	// Is the user logged on?  If not we can't get stats.
	if(!SteamUser()->BLoggedOn())
	{
		return false;
	}
	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}

//SetAchievement()
//Parameters - The string identifier of the Achievement that you want to set (ie. "ACH_WIN_ONE_GAME")
//Returns - a bool representing if the call succeeded or not. If the call failed then either Steam is not initialized or you still haven't processed the callback from the inital call to RequestStats(). You can't set any achievements until that callback has been received.
//What it does - This method sets a given achievement to achieved and sends the results to Steam. You can set a given achievement multiple times so you don't need to worry about only setting achievements that aren't already set. This is an asynchronous call which will trigger two callbacks: OnUserStatsStored() and OnAchievementStored().


bool CSteamAchievements::SetAchievement(const char* ID)
{
	// Have we received a call back from Steam yet?
	if (m_bInitialized)
	{
		SteamUserStats()->SetAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
	// If not then we can't set achievements yet
	return false;
}

//OnUserStatsReceived()
//Parameters - N/A
//Returns - Nothing
//What it does - This method is a callback that is called anytime you attempt to request stats. Stats are requested by using RequestStats(). The method updates the member variable m_Achievements to reflect the latest achievement data returned from Steam.


void CSteamAchievements::OnUserStatsReceived( UserStatsReceived_t *pCallback )
{
 // we may get callbacks for other games' stats arriving, ignore them
 if(m_iAppID == pCallback->m_nGameID )
 {
   if ( k_EResultOK == pCallback->m_eResult )
   {
     sysLogPrint("[Steam achievements] Received stats and achievements from Steam");
     m_bInitialized = true;

     // load achievements
     for ( int iAch = 0; iAch < m_iNumAchievements; ++iAch )
     {
       Achievement_t &ach = m_pAchievements[iAch];

       SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
       _snprintf( ach.m_rgchName, sizeof(ach.m_rgchName), "%s", 
          SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID, 
          "name"));
       _snprintf( ach.m_rgchDescription, sizeof(ach.m_rgchDescription), "%s", 
          SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID, 
          "desc"));			
     }
   }
   else
   {
     char buffer[128];
     _snprintf(buffer, 128, "RequestStats - failed, %d", pCallback->m_eResult );
     sysLogPrint("[Steam achievements] %s",buffer);
   }
 }
}

//OnUserStatsStored()
//Parameters - N/A
//Returns - Nothing
//What it does - This method is a callback that is called anytime you attempt to store stats on Steam.


void CSteamAchievements::OnUserStatsStored( UserStatsStored_t *pCallback )
{
 // we may get callbacks for other games' stats arriving, ignore them
 if ( m_iAppID == pCallback->m_nGameID )	
 {
   if ( k_EResultOK == pCallback->m_eResult )
   {
     sysLogPrint("[Steam achievements] Stored stats for Steam");
   }
   else
   {
     char buffer[128];
     _snprintf( buffer, 128, "StatsStored - failed, %d", pCallback->m_eResult );
     sysLogPrint("[Steam achievements] %s ",buffer);
   }
 }
}

//OnAchievementStored()
//Parameters - N/A
//Returns - Nothing
//What it does - This method is a callback that is called anytime Achievements are successfully stored on Steam.


void CSteamAchievements::OnAchievementStored( UserAchievementStored_t *pCallback )
{
     // we may get callbacks for other games' stats arriving, ignore them
     if ( m_iAppID == pCallback->m_nGameID )	
     {
          sysLogPrint("[Steam achievements] Stored Achievement for Steam");
     }
}

// LEADERBOARDS CODE
CSteamLeaderboards::CSteamLeaderboards(): 				
 m_CurrentLeaderboard( NULL ),
 m_nLeaderboardEntries( 0 )
{
}

void CSteamLeaderboards::FindLeaderboard( const char *pchLeaderboardName )
{
 m_CurrentLeaderboard = NULL;

 SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(pchLeaderboardName);
 m_callResultFindLeaderboard.Set(hSteamAPICall, this, 
   &CSteamLeaderboards::OnFindLeaderboard);
}

void CSteamLeaderboards::OnFindLeaderboard( LeaderboardFindResult_t *pCallback,	bool bIOFailure )
{
	// see if we encountered an error during the call
	if ( !pCallback->m_bLeaderboardFound || bIOFailure )
	{
		sysLogPrint("[Steam leaderboards] Leaderboard could not be found" );
		return;
	}
	sysLogPrint("[Steam leaderboards] Leaderboard found." );

	m_CurrentLeaderboard = pCallback->m_hSteamLeaderboard;
}

bool CSteamLeaderboards::UploadScore( int score )
{
 if (!m_CurrentLeaderboard)
 {
	 sysLogPrintDebug("CSteamLeaderboards::UploadScore() m_CurrentLeaderBoard = 0");
	return false;
 }

 SteamAPICall_t hSteamAPICall = 
   SteamUserStats()->UploadLeaderboardScore( m_CurrentLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, score, NULL, 0 );
	
 m_callResultUploadScore.Set(hSteamAPICall, this, &CSteamLeaderboards::OnUploadScore);

 return true;
}

void CSteamLeaderboards::OnUploadScore(LeaderboardScoreUploaded_t *pCallback, bool bIOFailure)
{
	if ( !pCallback->m_bSuccess || bIOFailure )
	{
		sysLogPrint("[Steam leaderboards] Score could not be uploaded to Steam\n" );
	}
	sysLogPrint("[Steam leaderboards] Score uploaded.\n" );
}

// STEAM Stats
CSteamStats::CSteamStats(Stat_t *Stats, int NumStats) :
 m_iAppID( 0 ),
 m_bInitialized( false ),
 m_CallbackUserStatsReceived( this, &CSteamStats::OnUserStatsReceived ),
 m_CallbackUserStatsStored( this, &CSteamStats::OnUserStatsStored )
{
	m_iAppID = SteamUtils()->GetAppID();
	m_pStats = Stats;
	m_iNumStats = NumStats;
	RequestStats();
}

bool CSteamStats::RequestStats()
{
	// Is Steam loaded? If not we can't get stats.
	if ( NULL == SteamUserStats() || NULL == SteamUser() )
	{
		return false;
	}
	// Is the user logged on?  If not we can't get stats.
	if ( !SteamUser()->BLoggedOn() )
	{
		return false;
	}
	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}

bool CSteamStats::StoreStats()
{
 if ( m_bInitialized )
 {
   // load stats
   for ( int iStat = 0; iStat < m_iNumStats; ++iStat )
   {
     Stat_t &stat = m_pStats[iStat];
     switch (stat.m_eStatType)
     {
       case STAT_INT:
         SteamUserStats()->SetStat(stat.m_pchStatName, stat.m_iValue);
         break;

       case STAT_FLOAT:
         SteamUserStats()->SetStat(stat.m_pchStatName, stat.m_flValue);
         break;

       case STAT_AVGRATE:
         SteamUserStats()->UpdateAvgRateStat(stat.m_pchStatName, 
           stat.m_flAvgNumerator, stat.m_flAvgDenominator );
         // The averaged result is calculated for us
         SteamUserStats()->GetStat(stat.m_pchStatName, &stat.m_flValue );
         break;

       default:
         break;
     }
   }
   return SteamUserStats()->StoreStats();
 }
    return 0;
}

void CSteamStats::OnUserStatsReceived( UserStatsReceived_t *pCallback )
{
 // we may get callbacks for other games' stats arriving, ignore them
 if ( m_iAppID == pCallback->m_nGameID )
 {
   if ( k_EResultOK == pCallback->m_eResult )
   {
     sysLogPrintDebug("[STEAM stats] Received stats and achievements from Steam\n" );
     // load stats
     for ( int iStat = 0; iStat < m_iNumStats; ++iStat )
     {
       Stat_t &stat = m_pStats[iStat];
       switch (stat.m_eStatType)
       {
         case STAT_INT:
           SteamUserStats()->GetStat(stat.m_pchStatName, &stat.m_iValue);
           break;

         case STAT_FLOAT:
         case STAT_AVGRATE:
           SteamUserStats()->GetStat(stat.m_pchStatName, &stat.m_flValue);
           break;

         default:
           break;
       }
     }
     m_bInitialized = true;
   }
   else
   {
     char buffer[128];
     _snprintf( buffer, 128, "RequestStats - failed, %d\n", pCallback->m_eResult );
     sysLogPrintDebug("[STEAM stats] %s",buffer );
   }
 }
}

void CSteamStats::OnUserStatsStored( UserStatsStored_t *pCallback )
{
 // we may get callbacks for other games' stats arriving, ignore them
 if ( m_iAppID == pCallback->m_nGameID )
 {
   if ( k_EResultOK == pCallback->m_eResult )
   {
     sysLogPrintDebug("[STEAM stats] StoreStats - success\n" );
   }
   else if ( k_EResultInvalidParam == pCallback->m_eResult )
   {
     // One or more stats we set broke a constraint. They've been reverted,
     // and we should re-iterate the values now to keep in sync.
     sysLogPrintDebug("[STEAM stats] StoreStats - some failed to validate\n" );
     // Fake up a callback here so that we re-load the values.
     UserStatsReceived_t callback;
     callback.m_eResult = k_EResultOK;
     callback.m_nGameID = m_iAppID;
     OnUserStatsReceived( &callback );
   }
   else
   {
     char buffer[128];
     _snprintf( buffer, 128, "StoreStats - failed, %d\n", pCallback->m_eResult );
     sysLogPrintDebug("[STEAM stats] %s", buffer );
   }
 }
}