#include "hedSystem.h"
#include "../addon - steam/steam_api.h"

#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }

struct Achievement_t
{
	int m_eAchievementID;
	const char *m_pchAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};


class CSteamAchievements
{
private:
	int64 m_iAppID; // Our current AppID
	Achievement_t *m_pAchievements; // Achievements data
	int m_iNumAchievements; // The number of Achievements
	bool m_bInitialized; // Have we called Request stats and received the callback?

public:
	CSteamAchievements(Achievement_t *Achievements, int NumAchievements);
	~CSteamAchievements();
	
	bool RequestStats();
	bool SetAchievement(const char* ID);

	STEAM_CALLBACK( CSteamAchievements, OnUserStatsReceived, UserStatsReceived_t, 
		m_CallbackUserStatsReceived );
	STEAM_CALLBACK( CSteamAchievements, OnUserStatsStored, UserStatsStored_t, 
		m_CallbackUserStatsStored );
	STEAM_CALLBACK( CSteamAchievements, OnAchievementStored, 
		UserAchievementStored_t, m_CallbackAchievementStored );
};

// STEAM Leaderboards

class CSteamLeaderboards
{
private:
	SteamLeaderboard_t m_CurrentLeaderboard; // Handle to leaderboard

public:
	int m_nLeaderboardEntries; // How many entries do we have?
	LeaderboardEntry_t m_leaderboardEntries[10]; // The entries

	CSteamLeaderboards();
	~CSteamLeaderboards(){};
	
	void FindLeaderboard( const char *pchLeaderboardName );
	bool UploadScore( int score );

	void OnFindLeaderboard( LeaderboardFindResult_t *pResult, bool bIOFailure);
	CCallResult<CSteamLeaderboards, LeaderboardFindResult_t> m_callResultFindLeaderboard;

	void OnUploadScore( LeaderboardScoreUploaded_t *pResult, bool bIOFailure);
	CCallResult<CSteamLeaderboards, LeaderboardScoreUploaded_t> m_callResultUploadScore;;
};

// STEAM stats
#define _STAT_ID( id,type,name ) { id, type, name, 0, 0, 0, 0 }

enum EStatTypes
{
	STAT_INT = 0,
	STAT_FLOAT = 1,
	STAT_AVGRATE = 2,
};

struct Stat_t
{
	int m_ID;
	EStatTypes m_eStatType;
	const char *m_pchStatName;
	int m_iValue;
	float m_flValue;
	float m_flAvgNumerator;
	float m_flAvgDenominator;
};

class CSteamStats
{
private:
	int64 m_iAppID; // Our current AppID
	int m_iNumStats; // The number of Stats
	bool m_bInitialized; // Have we called Request stats and received the callback?

public:
	Stat_t *m_pStats; // Stats data

	CSteamStats(Stat_t *Stats, int NumStats);
	~CSteamStats();

	bool RequestStats();
	bool StoreStats();

	STEAM_CALLBACK( CSteamStats, OnUserStatsReceived, UserStatsReceived_t, 
		m_CallbackUserStatsReceived );
	STEAM_CALLBACK( CSteamStats, OnUserStatsStored, UserStatsStored_t, 
		m_CallbackUserStatsStored );
};