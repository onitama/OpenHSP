
//
//	hspsteam.cpp structures
//
#ifndef __hspsteam_h
#define __hspsteam_h

#include "stdafx.h"
#include "membuf.h"

#pragma warning( push )
//  warning C4355: 'this' : used in base member initializer list
//  This is OK because it's warning on setting up the Steam callbacks, they won't use this until after construction is done
#pragma warning( disable : 4355 ) 

/*------------------------------------------------------------*/

enum {
	STEAM_NONE = 0,
	STEAM_ERROR,
	STEAM_READY,
	STEAM_GETSTAT,
	STEAM_MAX
};

#define STEAM_NOTICE_ACHIVEMENT 1
#define STEAM_NOTICE_STATS 2

#define STEAM_BUFSIZE 1024

struct Achievement_t
{
	int m_eAchievementID;
	const char *m_pchAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};

//
//	hspsteam.cpp functions
//

int _PrepareSteam(void);
void _ShutdownSteam(void);

class hspsteam {
public:
	hspsteam();
	~hspsteam();

	int	flag;			// STEAM_*
	int notice;			// STEAM_NOTICE_*

	STEAM_CALLBACK(hspsteam, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(hspsteam, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(hspsteam, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);

	void setAchivementMax(int max);
	int setAchivementAuto(void);
	void requestAchivement(void);
	void setAchivementId(int index, char *name);
	void unlockAchievement(int index);
	void unlockAchievement(char *apikey);
	void clearAchievement(int index);
	void clearAchievement(char *apikey);
	void update(void);
	char *getAchivementString(int id, int type);
	int getAchivementFlag(int index);

	void request_Leaderboard(char *name, int type);
	int request_LeaderboardData(int type,int n_start,int n_end);
	int getLeaderboardMax(void);
	int getLeaderboardValue(int type, int entry);
	int getLeaderboardString(char *result, int type, int entry);
	int getPlayerName(char *result, int type);
	int update_LeaderboardData(int value, int option);

	int getStatInt(char *apikey, int *result);
	int getStatDouble(char *apikey, double *result);
	void updateStatusInt(char *apikey, int value);
	void updateStatusDouble(char *apikey, double value);

	void setUTF8Mode(int mode);
	void setResultString(char *result, char *src);

private:
	//		Settings
	//
	int utf8mode;				// enable UTF-8 mode (0=SJIS/1=Native)

	// Steam User interface
	ISteamUser *m_pSteamUser;

	// Steam UserStats interface
	ISteamUserStats *m_pSteamUserStats;

	// Steam Leaderboard interface
	SteamLeaderboard_t m_CurrentLeaderboard;

	// our GameID
	CGameID m_GameID;

	// Achievement Data
	Achievement_t *g_rgAchievements;

	// Achievement Data Size
	int m_AchievementsSize;

	// Leaderboard Data
	LeaderboardEntry_t *m_leaderboardEntries;

	// Leaderboard Data Size
	int m_LeaderboardSize;

	// Called when SteamUserStats()->FindOrCreateLeaderboard() returns asynchronously
	void OnFindLeaderboard(LeaderboardFindResult_t *pFindLearderboardResult, bool bIOFailure);
	CCallResult<hspsteam, LeaderboardFindResult_t> m_SteamCallResultCreateLeaderboard;

	// Called when SteamUserStats()->DownloadLeaderboardEntries() returns asynchronously
	void OnDownloadScore(LeaderboardScoresDownloaded_t *pLeaderboardScoresDownloaded, bool bIOFailure);
	CCallResult<hspsteam, LeaderboardScoresDownloaded_t> m_callResultDownloadEntries;

	// Called when SteamUserStats()->UploadLeaderboardScore() returns asynchronously
	void OnUploadScore(LeaderboardScoreUploaded_t *pFindLearderboardResult, bool bIOFailure);
	CCallResult<hspsteam, LeaderboardScoreUploaded_t> m_SteamCallResultUploadScore;


	// Did we get the stats from Steam?
	bool m_bRequestedStats;
	bool m_bStatsValid;

	// Should we store stats this frame?
	bool m_bStoreStats;

	// true if we looking up a leaderboard handle
	bool m_bLoading;

	CMemBuf *namebuf;
};


#endif
