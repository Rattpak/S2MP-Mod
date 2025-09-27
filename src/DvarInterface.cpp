////////////////////////////////////////////
//          DVar Interface
// 
//  Convert DVar strings into enumerated 
//  DVar strings for S2MP
////////////////////////////////////////////
#include "pch.h"
#include "DvarInterface.hpp"
#include "Console.hpp"
#include "GameUtil.hpp"
#include "FuncPointers.h"
#include <algorithm>
#include "DevDef.h"

std::unordered_map<std::string, std::string> DvarInterface::userToEngineMap;
std::unordered_map<std::string, std::string> DvarInterface::engineToUserMap;

//Sets a Dvar in engine using the Dvar Interface
//Takes the name of the dvar, and the cmd vector
//Returns true if dvar was found and attempted to change
bool DvarInterface::setDvar(std::string& dvarname, std::vector<std::string> cmd) {
    std::string dvarLower = dvarname;
    std::transform(dvarLower.begin(), dvarLower.end(), dvarLower.begin(), GameUtil::asciiToLower);

    std::string engineString = DvarInterface::toEngineString(dvarLower);
    dvar_t* var = Functions::_Dvar_FindVar(engineString.c_str());
    if (!var) {
        return false;
    }

    std::string dvarCmd = "set " + engineString;
    for (int i = 1; i < cmd.size(); ++i) { //skip 0 cuz thats dvarname
        dvarCmd += " " + cmd[i];
    }

    GameUtil::Cbuf_AddText(LOCAL_CLIENT_0, dvarCmd.c_str());
    return true;
}
unsigned int DvarInterface::getDvarListSize() {
    return DvarInterface::userToEngineMap.size();
}

//Add a dvar mapping to the map
void DvarInterface::addMapping(const std::string& userString, const std::string& engineString) {
    std::string dvarLower = userString;
    std::transform(dvarLower.begin(), dvarLower.end(), dvarLower.begin(), GameUtil::asciiToLower);
    userToEngineMap[dvarLower] = engineString;
    engineToUserMap[engineString] = userString;
}

std::string DvarInterface::toEngineString(const std::string& userString) {
    std::string dvarLower = userString;
    std::transform(dvarLower.begin(), dvarLower.end(), dvarLower.begin(), GameUtil::asciiToLower);
    auto it = userToEngineMap.find(dvarLower);
    if (it != userToEngineMap.end()) {
       // Console::devPrint(userString + " ----> " + it->second);
        return it->second;
    }
    return userString; //couldnt find
}

std::string DvarInterface::toUserString(const std::string& engineString) {
    auto it = engineToUserMap.find(engineString);
    if (it != engineToUserMap.end()) {
        return it->second;
    }
    return engineString; //couldnt find
}

void DvarInterface::addDvarsWithName(const char* name) {
    addMapping(name, name);
}

void DvarInterface::addAllMappings() {
    //taking these from the generated user_config_mp.cfg
    //doing this since console pulls directly from dvar mapping
    addDvarsWithName("win_useWmInput");
    addDvarsWithName("com_maxfps");
    addDvarsWithName("com_graphicsTier");
    addDvarsWithName("com_setRecommendedPass");
    addDvarsWithName("ragdoll_enable");
    addDvarsWithName("r_lodScaleSkinned");
    addDvarsWithName("r_lodBiasSkinned");
    addDvarsWithName("r_lodScaleRigid");
    addDvarsWithName("r_lodBiasRigid");
    addDvarsWithName("r_drawWater");
    addDvarsWithName("ragdoll_mp_limit");
    addDvarsWithName("r_elevatedPriority");
    addDvarsWithName("r_preloadShaders");
    addDvarsWithName("r_preloadShadersELL");
    addDvarsWithName("r_preloadShadersELLMSPT");
    addDvarsWithName("r_preloadShadersWNDTOO");
    addDvarsWithName("r_smaaTemporalUpsamplePercent");
    addDvarsWithName("r_postAAMode");
    addDvarsWithName("sm_enable");
    addDvarsWithName("r_screenSpaceShadowsForce");
    addDvarsWithName("r_dof_limit");
    addDvarsWithName("r_mbLimit");
    addDvarsWithName("r_ssaoLimit");
    addDvarsWithName("r_mdaoLimit");
    addDvarsWithName("r_sssLimit");
    addDvarsWithName("r_texFilterAnisoMin");
    addDvarsWithName("r_videoMemoryScale");
    addDvarsWithName("vid_xpos");
    addDvarsWithName("vid_ypos");
    addDvarsWithName("vid_width");
    addDvarsWithName("vid_height");
    addDvarsWithName("r_fullscreen");
    addDvarsWithName("r_fullscreenWindow");
    addDvarsWithName("r_fullscreenWindowExtend");
    addDvarsWithName("r_fullscreenWindowExtendMode");
    addDvarsWithName("r_windowExtendFit");
    addDvarsWithName("r_vramOverheadFraction");
    addDvarsWithName("r_blacklevel");
    addDvarsWithName("r_hdrWhitePointLevel");
    addDvarsWithName("r_hdrBlackPointLevel");
    addDvarsWithName("r_hdrGammaCurveLevel");
    addDvarsWithName("r_texFilterAnisoMax");
    addDvarsWithName("r_picmip");
    addDvarsWithName("r_picmip_bump");
    addDvarsWithName("r_picmip_spec");
    addDvarsWithName("r_imageQuality");
    addDvarsWithName("r_vsync");
    addDvarsWithName("sm_cacheSpotShadows");
    addDvarsWithName("sm_cacheSunShadow");
    addDvarsWithName("sm_tileResolution");
    addDvarsWithName("r_skyResolution");
    addDvarsWithName("sm_sunShadowBitDepth");
    addDvarsWithName("sm_spotShadowBitDepth");
    addDvarsWithName("r_daltonizeMode");
    addDvarsWithName("r_daltonizeIntensity");
    addDvarsWithName("r_aspectRatio");
    addDvarsWithName("r_adapter");
    addDvarsWithName("r_monitor");
    addDvarsWithName("r_mode");
    addDvarsWithName("r_refreshRate");
    addDvarsWithName("r_ssrQualityLevel");
    addDvarsWithName("r_allowHDR");
    addDvarsWithName("r_ssrEnabled");
    addDvarsWithName("splitscreenMode");
    addDvarsWithName("cg_blood");
    addDvarsWithName("cg_clientViewAspect0");
    addDvarsWithName("cg_clientViewAspect1");
    addDvarsWithName("cg_fov");
    addDvarsWithName("cg_fov1");
    addDvarsWithName("cg_fov_intermission");
    addDvarsWithName("shadowplay_highlights_enabled");
    addDvarsWithName("shadowplay_killcam_highlights_enabled");
    addDvarsWithName("logitech_led");
    addDvarsWithName("ui_drawCrosshair");
    addDvarsWithName("ui_drawHitmarker");


    addMapping("nextmap", "4059");
    addMapping("mapname", "1673");
    //from R_RegisterDvars
    addMapping("r_texFilterDisable", "91");
    addMapping("r_texFilterMipMode", "4107");
    addMapping("r_texShowMipMode", "2416");
    addMapping("r_texFilterMipBias", "3569");
    addMapping("r_texFilterProbeBilinear", "5634");
    addMapping("r_lodDynamicScale", "1111");
    addMapping("r_artUseTweaks", "5960");
    addMapping("r_lightGridTempSmoothingFactor", "2677");
    addMapping("r_globalSecondarySelfVisScale", "1264");
    addMapping("r_globalSecondarySelfVisLerpToFullyOpen", "5139");
    addMapping("r_lightMap", "3271");
    addMapping("r_colorMap", "3391");
    addMapping("r_detailMap", "2194");
    addMapping("r_displacementMap", "4388");
    addMapping("r_normalMap", "5467");
    addMapping("r_specularMap", "2427");
    addMapping("r_specOccMap", "5620");
    addMapping("r_envBrdfLutMap", "3818");
    addMapping("r_emissiveMap", "769");
    addMapping("r_depthPrepass", "4600");
    addMapping("r_depthHackPrepass", "1342");
    addMapping("r_volumetricDepth", "3395");
    addMapping("r_forceLod", "3361");
    addMapping("r_useAerialLod", "1841");
    addMapping("sm_spotEnable", "2030");
    addMapping("sm_spotLightScoreRadiusPower", "1475");
    addMapping("sm_dynlightAllSModels", "5657");
    addMapping("sm_sunShadowBoundsOverride", "1064");
    addMapping("sm_sunShadowBoundsMin", "2611");
    addMapping("sm_sunShadowBoundsMax", "419");
    addMapping("sm_spotShadowMapSize", "935");
    addMapping("sm_sunPolygonOffsetScale", "2428");
    addMapping("sm_sunPolygonOffsetBias", "5142");
    addMapping("sm_sunPolygonOffsetClamp", "3266");
    addMapping("sm_spotPolygonOffsetScale", "4087");
    addMapping("sm_spotPolygonOffsetBias", "3357");
    addMapping("sm_spotPolygonOffsetClamp", "136");
    addMapping("sm_sunShadowCenter", "450");
    addMapping("sm_sunShadowCenterMode", "775");
    addMapping("r_filmGrainUseTweaks", "4316");
    addMapping("r_filmGrainIntensity", "4276");
    addMapping("r_filmGrainFps", "3289");
    addMapping("r_postfx_enable", "5605");
    addMapping("r_sunPostDrawBeforeTrans", "849");
    addMapping("r_dof_enable", "2046");
    addMapping("r_dof_tweak", "2");
    addMapping("r_dof_nearBlur", "2208");
    addMapping("r_dof_farBlur", "1256");
    addMapping("r_dof_viewModelStart", "231");
    addMapping("r_dof_viewModelEnd", "5414");
    addMapping("r_dof_nearStart", "875");
    addMapping("r_dof_nearEnd", "2580");
    addMapping("r_dof_farStart", "756");
    addMapping("r_dof_farEnd", "4058");
    addMapping("r_dof_bias", "97");
    addMapping("r_dof_physical_filmDiagonal", "4419");
    addMapping("r_dof_physical_hipEnable", "1972");
    addMapping("r_dof_physical_hipFstop", "2186");
    addMapping("r_dof_physical_hipSharpCocDiameter", "275");//ayo?
    addMapping("r_dof_physicalHipFocusSpeed", "5398");
    addMapping("r_dof_physical_fstop", "3380");
    addMapping("r_dof_physical_focusDistance", "1920");
    addMapping("r_dof_physical_viewModelFstop", "1077");
    addMapping("r_dof_physical_viewModelFocusDistance", "1052");
    addMapping("r_dof_physical_adsFocusSpeed", "4854");
    addMapping("r_dof_physical_adsMinFstop", "4242");
    addMapping("r_dof_physical_adsMaxFstop", "1315");
    addMapping("r_dof_physical_minFocusDistance", "2386");
    addMapping("r_dof_physical_maxCocDiameter", "2888");//AYO
    addMapping("r_colorGradingEnable", "523");
    addMapping("r_colorimetrySDRForcePQ", "5246");
    addMapping("r_colorimetryHDRForcePQ", "5322");
    addMapping("r_colorimetryHDRDisableBlackLevelAdjust", "4268");
    addMapping("r_colorimetryHDRExposureAdjust", "2268");
    addMapping("r_outdoorFeather", "161");
    addMapping("r_sun_from_dvars", "2255");
    addMapping("r_atlasAnimFPS", "1289");
    addMapping("developer", "1147");
    addMapping("r_tessellationHeightAuto", "75");
    addMapping("r_tessellationHeightScale", "672");
    addMapping("r_tessellationHybrid", "551");
    addMapping("r_tessellationEyeScale", "330");
    addMapping("r_offchipTessellationAllowed", "1938");
    addMapping("r_offchipTessellationTfThreshold", "635");
    addMapping("r_offchipTessellationWaveThreshold", "4679");
    addMapping("r_patchCountAllowed", "3925");
    addMapping("r_subdivPatchCount", "4902");
    addMapping("r_displacementPatchCount", "3287");
    addMapping("r_defaultPatchCount", "3923");
    addMapping("r_lateAllocParamCacheAllowed", "1518");
    addMapping("r_eyeSparkle", "727");
    addMapping("r_eyePupil", "1932");
    addMapping("r_eyeRedness", "3458");
    addMapping("r_eyeRednessVeins", "1845");
    addMapping("r_eyeHighlightIntensity", "4065");
    addMapping("r_eyeHighlightColor", "5421");
    addMapping("r_foliageWindMaterialParams", "842");
    addMapping("r_rimLightNearEdgeSharpness", "2315");
    addMapping("r_rimLightFarEdgeSharpness", "2403");
    addMapping("r_rimLightSharpnessDistance", "1079");
    addMapping("r_rimLightIntensity", "5482");
    addMapping("r_rimLightShadowBrightening", "3816");
    addMapping("r_rimLightBias", "5332");
    addMapping("r_rimLightFalloffMaxDistance", "5574");
    addMapping("r_rimLightFalloffMinDistance", "915");
    addMapping("r_keyLightStrength", "2910");
    addMapping("r_keyLightDistanceMax", "1603");
    addMapping("r_keyLightDistanceMin", "1878");
    addMapping("r_keyLightSpecScale", "4409");
    addMapping("r_keyLightDirection", "3142");
    addMapping("r_keyLightAxisCharColor", "1937");
    addMapping("r_keyLightAlliesCharColor", "5018");
    addMapping("r_rimLightUseTweaks", "4267");
    addMapping("r_airLightScoreRadiusPower", "564");
    addMapping("r_airLightScoreMinimum", "2258");
    addMapping("r_airLightVertexInnerMutedRadius", "2981");
    addMapping("r_airLightBlendTime", "4013");
    addMapping("r_airLightUpsampleDepthMaximum", "1355");
    addMapping("r_airLightUpsampleDepthThresholdAngle", "4688");
    addMapping("r_airLightForceLQ", "2697");
    addMapping("r_airLightForceHQ", "1432");
    addMapping("r_clutCompositeVisionSet", "855");
    addMapping("r_depthSortEnable", "1114");
    addMapping("r_depthSortRange", "1414");
    addMapping("r_ssrPositionCorrection", "4924");
    addMapping("r_hudOutlineEnable", "2976");
    addMapping("r_hudOutlinePostMode", "1394");
    addMapping("r_hudOutlineWhen", "2903");
    addMapping("r_hudOutlineWidth", "5539");
    addMapping("r_hudOutlineWidthBroadcaster", "3248");
    addMapping("r_hudOutlineAlpha0", "31");
    addMapping("r_hudOutlineAlpha0Broadcaster", "5531");
    addMapping("r_hudOutlineAlpha1", "1070");
    addMapping("r_hudOutlineAlpha1Broadcaster", "978");
    addMapping("r_hudOutlineHaloWhen", "593");
    addMapping("r_hudOutlineHaloBlurRadius", "2197");
    addMapping("r_hudOutlineHaloLumScale", "1173");
    addMapping("r_hudOutlineHaloDarkenScale", "726");
    addMapping("r_hudOutlineCurvyWhen", "3187");
    addMapping("r_hudOutlineCurvyBlurRadius", "1486");
    addMapping("r_hudOutlineCurvyWidth", "710");
    addMapping("r_hudOutlineCurvyDepth", "4008");
    addMapping("r_hudOutlineCurvyLumScale", "961");
    addMapping("r_hudOutlineCurvyDarkenScale", "4501");
    addMapping("r_hudOutlineCloakWhen", "1018");
    addMapping("r_hudOutlineCloakBlurRadius", "2530");
    addMapping("r_hudOutlineCloakLumScale", "2362");
    addMapping("r_hudOutlineCloakDarkenScale", "4701");
    addMapping("r_fogScaleEnable", "5217");
    addMapping("r_fogScale", "5051");
    addMapping("r_chromaticAberration", "3588");
    addMapping("r_chromaticSeparationR", "4160");
    addMapping("r_chromaticSeparationG", "1816");
    addMapping("r_chromaticSeparationB", "2185");
    addMapping("r_chromaticAberrationAlpha", "700");
    addMapping("r_chromaticAberrationTweaks", "4946");
    addMapping("r_chromaticPostOpaqueFX", "2083");
    addMapping("r_frustomLightUseZBinning", "2244");
    addMapping("r_lightDpvs", "3825");
    addMapping("r_forwardPlusMode", "5818");

    //from CG_CompassRegisterDvars
    addMapping("compassSize", "2692");
    addMapping("compassSoundPingFadeTime", "5111");
    addMapping("compassClampIcons", "2980");
    addMapping("compassFriendlyWidth", "4963");
    addMapping("compassFriendlyHeight", "784");
    addMapping("compassPlayerWidth", "3340");
    addMapping("compassPlayerHeight", "2430");
    addMapping("compassRotation", "4433");
    addMapping("compassTickertapeStretch", "66");
    addMapping("compassRadarPingFadeTime", "4811");
    addMapping("compassRadarLineThickness", "3967");
    addMapping("compassObjectiveArrowHeight", "660");
    addMapping("compassObjectiveMaxRange", "3565");
    addMapping("compassObjectiveMinAlpha", "5258");
    addMapping("compassObjectiveIconWidth", "2483");
    addMapping("compassObjectiveIconHeight", "2617");
    addMapping("compassObjectiveDetailDist", "3934");
    addMapping("compassPrototypeElevation", "3500");
    addMapping("compassPrototypeFiring", "4129");
    addMapping("compassHideVehicles", "965");
    addMapping("cg_hudMapRadarLineThickness", "4365");
    addMapping("cg_hudMapFriendlyWidth", "796");
    addMapping("cg_hudMapFriendlyHeight", "714");
    addMapping("cg_hudMapPlayerWidth", "4685");
    addMapping("cg_hudMapPlayerHeight", "1428");
    addMapping("cg_hudMapBorderWidth", "701");
    addMapping("cg_compassTrailExplosiveNumItems", "1914");
    addMapping("cg_compassTrailExplosiveItemDist", "4045");
    addMapping("cg_compassTrailExplosiveFadeDur", "1030");
    addMapping("cg_compassTrailExplosiveMinScale", "4718");
    addMapping("cg_compassTrailExplosiveMaxScale", "2624");
    addMapping("cg_compassTrailExplosiveRandScaleRange", "3789");
    addMapping("cg_compassTrailExplosiveRandScaleOffset", "4641");
    addMapping("cg_compassTrailFireNumItems", "2036");
    addMapping("cg_compassTrailFireItemDist", "4244");
    addMapping("cg_compassTrailFireFadeDur", "5604");
    addMapping("cg_compassTrailFireMinScale", "4997");
    addMapping("cg_compassTrailFireMaxScale", "35");
    addMapping("cg_compassTrailFireRandScaleRange", "3497");
    addMapping("cg_compassTrailFireRandScaleOffset", "1082");
    addMapping("cg_compassScorestreakUseSelfColor", "5837");
    addMapping("cg_compassEnemyDrawMode", "4530");
    addMapping("cg_compassClippedEnemyIconScale", "3607");
    addMapping("cg_compassClippedEnemyDistanceClose", "2283");
    addMapping("cg_compassClippedEnemyDistanceMedium", "92");
    addMapping("cg_compassClippedEnemyAlphaClose", "509");
    addMapping("cg_compassClippedEnemyAlphaMedium", "1022");
    addMapping("cg_compassClippedEnemyAlphaFar", "613");
    addMapping("motionTrackerPingFadeTime", "3531");
    addMapping("motionTrackerBlurDuration", "1186");
    addMapping("motionTrackerPingSize", "5793");
    addMapping("motionTrackerCenterX", "182");
    addMapping("motionTrackerCenterY", "5006");
    addMapping("motionTrackerPingPitchBase", "679");
    addMapping("motionTrackerPingPitchNearby", "5435");
    addMapping("motionTrackerPingPitchAddPerEnemy", "5477");

    //from SocialConfig_Init
    addMapping("theater_active", "616");
    addMapping("facebook_active", "4761");
    addMapping("facebook_delay", "1636");
    addMapping("facebook_max_retry_time", "5797");
    addMapping("facebook_retry_step", "4142");
    addMapping("facebook_friends_max_retry_time", "339");
    addMapping("facebook_friends_retry_step", "2306");
    addMapping("facebook_friends_refresh_time", "5238");
    addMapping("facebook_friends_throttle_time", "5232");
    addMapping("facebook_friends_active", "2687");
    addMapping("facebook_upload_video_active", "2688");
    addMapping("facebook_upload_photo_active", "5141");
    addMapping("userGroup_active", "1021");
    addMapping("userGroup_max_retry_time", "1490");
    addMapping("userGroup_retry_step", "1419");
    addMapping("userGroup_RetryTime", "227");
    addMapping("userGroup_refresh_time_secs", "55");
    addMapping("userGroup_cool_off_time", "776");
    addMapping("elite_clan_delay", "2672");
    addMapping("elite_clan_active", "3629");
    addMapping("elite_clan_get_clan_max_retry_time", "4416");
    addMapping("elite_clan_get_clan_retry_step", "1092");
    addMapping("elite_clan_get_members_max_retry_time", "4619");
    addMapping("elite_clan_get_members_retry_step", "2452");
    addMapping("elite_clan_get_blob_profile_max_retry_time", "4136");
    addMapping("elite_clan_get_blob_profile_retry_step", "813");
    addMapping("elite_clan_get_public_profile_max_retry_time", "1574");
    addMapping("elite_clan_get_public_profile_retry_step", "2257");
    addMapping("elite_clan_upload_emblemunlock_enable", "2849");
    addMapping("elite_clan_get_private_member_profile_max_retry_time", "53");
    addMapping("elite_clan_get_private_member_profile_retry_step", "834");
    addMapping("elite_clan_set_private_member_profile_max_retry_time", "636");
    addMapping("elite_clan_set_private_member_profile_retry_step", "2655");
    addMapping("elite_clan_send_message_to_members_max_retry_time", "643");
    addMapping("elite_clan_send_message_to_members_rerty_step", "4237");
    addMapping("elite_clan_cool_off_time", "5281");
    addMapping("elite_clan_motd_throttle_time", "2209");
    addMapping("elite_clan_remote_view_active", "5205");
    addMapping("elite_clan_remote_view_retry_step", "434");
    addMapping("elite_clan_remote_view_max_retry_time", "5560");
    addMapping("dw_presence_active", "4174");
    addMapping("dw_presence_coop_join", "418");
    addMapping("dw_presence_put_delay", "3550");
    addMapping("dw_presence_put_rate", "561");
    addMapping("dw_presence_get_rate", "3602");
    addMapping("dw_interleave_all_nat_types", "3358");
    addMapping("dw_enable_connection_telemetry", "1799");
    addMapping("dw_nattrav_cache_enable", "770");
    addMapping("dw_nattrav_cache_timeout", "463");
    addMapping("num_available_map_packs", "5942");
    addMapping("clientNetPerf_enabled", "1427");
    addMapping("clientNetPerf_UserCmdTimeWindowMs", "156");
    addMapping("clientNetPerf_UserCmdProcessedMinCount", "5040");
    addMapping("clientNetPerf_UserCmdQueuedMinCount", "4912");
    addMapping("clientNetPerf_UserCmdDroppedMinCount", "2614");
    addMapping("sv_clientPacketsBurstMinCount", "2044");
    addMapping("iotd_active", "2623");
    addMapping("iotd_retry", "60");
    addMapping("igs_td", "532");
    addMapping("matchdata_active", "967");
    addMapping("matchdata_maxcompressionbuffer", "2660");
    addMapping("breadcrumbdata_active", "466");
    addMapping("breadcrumbdata_maxcompressionbuffer", "4030");
    addMapping("breadcrumbdata_frequency_seconds", "34");
    addMapping("spawndata_active", "1745");
    addMapping("spawndata_maxcompressionbuffer", "2149");
    addMapping("matchnetperf_active", "109");
    addMapping("playercard_cache_validity_life", "5175");
    addMapping("playercard_cache_upload_max_retry_time", "3644");
    addMapping("playercard_cache_upload_retry_step", "2476");
    addMapping("playercard_cache_download_max_retry_time", "301");
    addMapping("playercard_cache_download_retry_step", "3347");
    addMapping("match_making_telemetry_chance", "743");
    addMapping("log_host_migration_chance", "2574");
    addMapping("max_ping_threshold_good", "4332");
    addMapping("max_ping_threshold_medium", "4760");
    addMapping("aci", "160");
    addMapping("vpte", "5029");
    addMapping("zombiesAllowSoloPause", "5353");
    addMapping("dlog_active", "5785");
    addMapping("marketing_refresh_time", "1543");
    addMapping("emblems_active", "4798");
    addMapping("selfie_active", "387");
    addMapping("ca_intra_only", "3302");
    addMapping("ca_do_mlc", "1966");
    addMapping("ca_require_signin", "2520");
    addMapping("ca_auto_signin", "5570");
    addMapping("lb_times_in_window", "3355");
    addMapping("lb_window", "2163");
    addMapping("svwp", "765");
    addMapping("dc_lobbymerge", "3583");
    addMapping("net_write_tween_packets", "5801");
    addMapping("net_read_tween_packets", "4773");
    addMapping("net_latest_tween_threshold", "1907");
    addMapping("ae_minBufferingTimeForHFEvents", "1778");
    addMapping("ae_forceAllEventsDispatchType", "25");
    addMapping("ae_dropGameEventsWithDispatchType", "5805");
    addMapping("ae_dropGameEventsWithID", "5826");
    addMapping("lfg_enabled", "2195");
    addMapping("lfg_playlist_search_offset", "4609");
    addMapping("lfg_freeslot_advertise_cutoff", "3008");

    //some lui and hub jawns
    //also a bunch of E3 stuff
    addMapping("lui_menuFlowEnabled", "4436");
    addMapping("lui_xboxlive_menu", "3547");
    addMapping("lui_systemlink_menu", "1572");
    addMapping("lui_splitscreenupscaling", "659");
    addMapping("lui_draw_hints", "5270");
    addMapping("e3demo", "2803");
    addMapping("e3demo_host", "4605");
    addMapping("e3demo_client", "1303");
    addMapping("e3demo_show_client_title_screen", "871");
    addMapping("lui_hud_motion_enabled", "42");
    addMapping("lui_hud_motion_perspective", "5345");
    addMapping("lui_hud_motion_translation_scale", "381");
    addMapping("lui_hud_motion_translation_max", "4686");
    addMapping("lui_hud_motion_rotation_scale", "386");
    addMapping("lui_hud_motion_rotation_max", "1033");
    addMapping("lui_hud_motion_bob_scale", "5406");
    addMapping("lui_hud_motion_angle_ease_speed", "2690");
    addMapping("lui_hud_motion_trans_ease_speed", "397");
    addMapping("hub_vendor_overhead_min_distance", "1647");
    addMapping("hub_vendor_overhead_max_distance", "3924");
    addMapping("hub_supply_drop_max_distance", "4143");
    addMapping("hub_leaderboard_max_distance", "3296");

    addMapping("cg_foliagesnd_alias", "4011");
    addMapping("cg_broadcasterSkycamDistance", "3119");
    addMapping("cg_subtitleForcedColor", "5004");
    addMapping("cg_subtitleColor", "2191");
    addMapping("cg_gunReticleTeamColor_EnemyTeam", "4426");
    addMapping("cg_gunReticleTeamColor_MyTeam", "4893");
    addMapping("cg_ScorestreakColor_Enemy", "2906");
    addMapping("cg_weaponVisInterval", "412");
    addMapping("cg_disableScreenShake", "3926");
    addMapping("useRelativeTeamColors", "1244");
    addMapping("cg_weapHitCullEnable", "5446");
    addMapping("cg_weapHitCullAngle", "207");
    addMapping("overrideNVGModelWithKnife", "5025");
    addMapping("cg_viewZSmoothingTime", "957");
    addMapping("cg_viewZSmoothingMax", "2391");
    addMapping("cg_viewZSmoothingMin", "5640");
    addMapping("cg_invalidCmdHintBlinkInterval", "811");
    addMapping("cg_invalidCmdHintDuration", "641");
    addMapping("cg_flashbangNameFadeOut", "3428");
    addMapping("cg_flashbangNameFadeIn", "5860");
    addMapping("cg_friendlyNameFadeOut", "5344");
    addMapping("cg_friendlyNameFadeIn", "3768");
    addMapping("cg_drawFriendlyNames", "1380");
    addMapping("cg_overheadNamesFont", "731");
    addMapping("cg_overheadNamesGlow", "257");
    addMapping("cg_overheadNamesFarScale", "1900");
    addMapping("cg_overheadNamesFarDist", "2393");
    addMapping("cg_overheadNamesNearDist", "1733");

    addMapping("com_errorResolveCommand", "4278");
    
    addMapping("r_portalMinClipArea", "634");
    addMapping("r_portalMinRecurseDepth", "3100");
    addMapping("r_sunshadowmap_cmdbuf_worker", "5260");
    addMapping("r_animatedVertsUseNoCacheLimit", "2274");
    addMapping("r_animatedVertsNoCacheScale", "3220");
    addMapping("r_screenSpaceShadows", "4712");
    addMapping("r_maxScreenSpaceShadowsSamplesTotal", "4241");
    addMapping("r_minScreenSpaceShadowsSamplesPerLight", "4941");
    addMapping("r_volumeLightScatter", "2696");
    addMapping("r_useLightGridDefaultModelLightingLookup", "2706");
    addMapping("r_lightGridDefaultModelLightingLookup", "3460");
    addMapping("r_useLightGridDefaultFXLightingLookup", "1074");
    addMapping("r_lightGridDefaultFXLightingLookup", "1307");
    addMapping("r_blurdstGaussianBlurRadius", "190");
    addMapping("r_blurdstGaussianBlurLevel", "3261");
    addMapping("r_uiBlurDstMode", "1175");
    addMapping("sm_spotLightScoreModelScale", "4230");
    addMapping("sm_minSpotLightScore", "5176");
    addMapping("sm_spotShadowFadeTime", "3286");
    addMapping("r_useShadowGeomOpt", "4163");
    addMapping("r_fog_depthhack_scale", "645");
    addMapping("r_materialWind", "1520");
    addMapping("r_mpRimDiffuseTint", "817");
    addMapping("r_mpRimStrength", "2634");
    addMapping("r_mpRimColor", "2013");
    addMapping("fx_enable", "3917");

    addMapping("cg_drawCrosshair", "1874");
    addMapping("cg_drawCrosshairNames", "1979");
    addMapping("cg_hudGrenadeIconMaxRangeFrag", "787");
    addMapping("cg_drawFriendlyNamesAlways", "4934");
    addMapping("cg_drawFriendlyHUDGrenades", "4466");

    addMapping("camera_thirdPerson", "311");

    addMapping("r_warningRepeatDelay", "1371");

    addMapping("r_lockPvs", "1897");
    addMapping("r_skipPvs", "2745");
    addMapping("r_portalBevels", "5317");
    addMapping("r_portalBevelsOnly", "3948");
    addMapping("r_portalWalkLimit", "2909");
    addMapping("r_materialLodOverride", "5148");
    addMapping("r_materialLodMin", "3389");
    addMapping("r_materialLod0SizeThreshold", "1099");
    addMapping("r_dynamicSpotLightShadows", "4739");

    //BG_RegisterDvars
    addMapping("bg_shock_soundLoop", "1270");
    addMapping("bg_shock_soundLoopSilent", "753");
    addMapping("bg_shock_soundEnd", "2839");
    addMapping("bg_shock_soundEndAbort", "1455");
    addMapping("bg_shock_screenType", "2748");
    addMapping("bg_shock_screenBlurBlendTime", "4103");
    addMapping("bg_shock_screenBlurBlendFadeTime", "5644");
    addMapping("bg_shock_screenFlashWhiteFadeTime", "5375");
    addMapping("bg_shock_screenFlashShotFadeTime", "4531");
    addMapping("bg_shock_viewKickPeriod", "3856");
    addMapping("bg_shock_viewKickRadius", "3850");
    addMapping("bg_shock_viewKickFadeTime", "1407");
    addMapping("bg_shock_fadeOverride", "3712");
    addMapping("bg_shock_sound", "4269");
    addMapping("bg_shock_soundFadeInTime", "304");
    addMapping("bg_shock_soundFadeOutTime", "2931");
    addMapping("bg_shock_soundLoopFadeTime", "954");
    addMapping("bg_shock_soundLoopEndDelay", "3375");
    addMapping("bg_shock_soundRoomType", "3620");
    addMapping("bg_shock_soundDryLevel", "1262");
    addMapping("bg_shock_soundWetLevel", "1397");
    addMapping("bg_shock_soundModEndDelay", "2010");
    addMapping("bg_shock_soundSubmix", "3744");
    addMapping("bg_shock_lookControl", "1607");
    addMapping("bg_shock_lookControl_maxpitchspeed", "2670");
    addMapping("bg_shock_lookControl_maxyawspeed", "667");
    addMapping("bg_shock_lookControl_mousesensitivityscale", "1378");
    addMapping("bg_shock_lookControl_fadeTime", "1824");
    addMapping("bg_shock_movement", "4216");
    addMapping("hudOutlineDuringADS", "2260");
    addMapping("combatRolesEnabled", "1936");
    addMapping("clientSideEffects", "3508");
    addMapping("cg_debugRootMotionLog", "3343");

    addMapping("riotshield_deployed_health", "650");
    addMapping("riotshield_deploy_trace_parallel", "4101");

    //CG_MissileRegisterDvars
    addMapping("cameraShakeRemoteMissile_Angles", "4141");
    addMapping("cameraShakeRemoteMissile_Freqs", "4275");
    addMapping("cameraShakeRemoteMissile_SpeedRange", "5243"); //vec2
    addMapping("cameraShakeRemoteHelo_Angles", "3086");
    addMapping("cameraShakeRemoteHelo_Freqs", "4780");
    addMapping("cameraShakeRemoteHelo_SpeedRange", "1157"); //vec2
    addMapping("missileRemoteFOV", "2727");
    addMapping("missileGlideBombRotationFallingRate", "4644");
    addMapping("missileGlideBombRotationFallingDelay", "2447");

    //CG_HudElemRegisterDvars
    addMapping("waypointDebugDraw", "4610");
   // addMapping("waypointIconWidth", "2427"); //typo
    //addMapping("waypointIconHeight", "2785");
    addMapping("waypointOffscreenPointerWidth", "4819");
    addMapping("waypointOffscreenPointerHeight", "1737");
    addMapping("waypointOffscreenPointerDistance", "5113");
    addMapping("waypointOffscreenDistanceThresholdAlpha", "964");
    addMapping("waypointOffscreenPadLeft", "2463");
    addMapping("waypointOffscreenPadRight", "833");
    addMapping("waypointOffscreenPadTop", "1039");
    addMapping("waypointOffscreenPadBottom", "1040");
    addMapping("waypointOffscreenRoundedCorners", "998");
    addMapping("waypointOffscreenCornerRadius", "1684");
    addMapping("waypointOffscreenScaleLength", "4579");
    addMapping("waypointOffscreenScaleSmallest", "5102");
    addMapping("waypointDistScaleRangeMin", "4204");
    addMapping("waypointDistScaleRangeMax", "1739");
    addMapping("waypointDistScaleSmallest", "5561");
    addMapping("waypointDistFadeRangeMin", "5209");
    addMapping("waypointDistFadeRangeMax", "5649");
    addMapping("waypointSplitscreenScale", "2865");
    addMapping("waypointScreenCenterFadeRadius", "2587");
    addMapping("waypointScreenCenterFadeAdsMin", "3253");
    addMapping("waypointScreenCenterFadeHipMin", "3935");
    addMapping("waypointTweakY", "4867");
    addMapping("hudElemPausedBrightness", "4520");
    addMapping("waypointPlayerOffsetProne", "3928");
    addMapping("waypointPlayerOffsetCrouch", "3463");
    addMapping("waypointPlayerOffsetStand", "3848");
    addMapping("objectiveFontSize", "5129");
    addMapping("objectiveTextOffsetY", "599");
    addMapping("genericIconAlpha", "4783");
    addMapping("enericIconAlphaFadeTime", "3630");
    addMapping("objectiveHide", "432");
    addMapping("waypointAerialIconScale", "3096");
    addMapping("waypointAerialIconMinSize", "3441");
    addMapping("waypointAerialIconMaxSize", "1254");
    addMapping("cg_foliagesnd_alias", "4011");

    //CG_RegisterVisionSetsDvars
    addMapping("nightVisionFadeInOutTime", "4625");
    addMapping("nightVisionPowerOnTime", "4842");
    addMapping("nightVisionDisableEffects", "3628");

    //CG_AmmoCounterRegisterDvars
    addMapping("lowAmmoWarningColor1", "646");
    addMapping("lowAmmoWarningColor2", "5124");
    addMapping("lowAmmoWarningPulseFreq", "5703");
    addMapping("lowAmmoWarningPulseMax", "11");
    addMapping("lowAmmoWarningPulseMin", "1036");
    addMapping("lowAmmoWarningNoReloadColor1", "3535");
    addMapping("lowAmmoWarningNoReloadColor2", "173");
    addMapping("lowAmmoWarningNoAmmoColor1", "5600");
    addMapping("lowAmmoWarningNoAmmoColor2", "5490");

    //CG_VectorField_RegisterDvars
    addMapping("cg_vectorFieldsForceUniform", "2895");

    //phys
    addMapping("phys_autoDisableTime", "5130");
    addMapping("phys_bulletUpBias", "2295");
    addMapping("phys_bulletSpinScale", "4747");
    addMapping("phys_dragAngular", "100");
    addMapping("phys_dragLinear", "1522");
    addMapping("phys_gravity", "4290");
    addMapping("physVeh_jump", "713");

    //LUI
    addMapping("LUI_WorkerCmdGC", "3886");

    //ComScore_Init
    addMapping("comscore_backoff", "3219");

    addMapping("dive_exhaustion_window", "4075");
    addMapping("bg_shieldHitEncodeWidthWorld", "3901");
    addMapping("bg_shieldHitEncodeHeightWorld", "1612");
    addMapping("bg_shieldHitEncodeWidthVM", "4078");
    addMapping("bg_shieldHitEncodeHeightVM", "1140");

    addMapping("cg_killCamTurretLerpTime", "5831");
    addMapping("cg_killCamDefaultLerpTime", "2254");
    addMapping("cg_descriptiveText", "4666");
    addMapping("cg_hearVictimTime", "408");
    addMapping("cg_hearVictimEnabled", "3452");
    addMapping("cg_gameBoldMessageWidth", "4916");
    addMapping("cg_gameMessageWidth", "5279");
    addMapping("cg_subtitleWidthWidescreen", "1003");
    addMapping("cg_subtitleWidthStandard", "4834");
    addMapping("cg_subtitleMinTime", "2986");
    addMapping("cg_drawpaused", "3496");
    addMapping("cl_paused", "183");
    addMapping("cg_teamChatsOnly", "1127");
    addMapping("cg_chatHeight", "2662");
    addMapping("cg_chatTime", "804");
    addMapping("tracer_stoppingPowerWidth", "2824");
    addMapping("tracer_stoppingPowerColor1", "3726");
    addMapping("tracer_stoppingPowerColor2", "2658");
    addMapping("tracer_stoppingPowerColor3", "2410");
    addMapping("tracer_stoppingPowerColor4", "5169");
    addMapping("tracer_stoppingPowerColor5", "3870");
    addMapping("tracer_stoppingPowerOverride", "4844");
    addMapping("tracer_firstPersonMaxWidth", "886");
    addMapping("tracer_thermalWidthMult", "4930");
    addMapping("cg_marks_ents_player_only", "4775");
    addMapping("cg_landingSounds", "4440");
    addMapping("cg_footsteps", "4291");
    addMapping("cg_errordecay", "4391");
    addMapping("cg_processImmediateEvents", "2921");
    addMapping("cg_tweenExtrapolationPeriodMs", "5173");
    addMapping("cg_tweenOverrideThresholdMs", "4088");
    addMapping("cg_tweenOverridePeriodSlowMs", "2778");
    addMapping("cg_tweenOverridePeriodMs", "1906");
    addMapping("cg_brass", "3385");
    addMapping("cg_crosshairEnemyColor", "3165");
    addMapping("cg_crosshairDynamic", "2700");
    addMapping("cg_crosshairAlphaMin", "4967");
    addMapping("cg_crosshairAlpha", "2105");
    addMapping("cg_weaponCycleDelay", "3891");
    addMapping("cg_hudSplitscreenCompassElementScale", "4041");
    addMapping("cg_hudSplitscreenCompassScale", "1888");
    addMapping("cg_mapLocationSelectionCursorSpeed", "281");
    addMapping("cg_hudSayPosition", "2815");
    addMapping("cg_hudChatIntermissionPosition", "4228");
    addMapping("cg_hudChatPosition", "2563");
    addMapping("cg_hudGrenadeIconEnabledFlash", "4876");
    addMapping("cg_hudGrenadeIconInScope", "5259");
    addMapping("cg_hudGrenadeIconMaxRangeFlash", "4356");
    addMapping("cg_hudDamageIconInScope", "4366");
    addMapping("cg_hudDamageIconTime", "4869");
    addMapping("cg_hudDamageIconHeight", "4434");
    addMapping("cg_hudDamageIconWidth", "2858");
    addMapping("painVisionLerpOutRate", "84");
    addMapping("painVisionTriggerHealth", "2349");
    addMapping("cg_drawDamageDirection", "883");
    addMapping("cg_drawDamageFlash", "221");
    addMapping("cg_drawTurretCrosshair", "5771");
    addMapping("cg_drawMantleHint", "2104");
    addMapping("cg_drawDoubleTapDetonateHint", "1784");
    addMapping("cg_drawBreathHUD", "1668");
    addMapping("cg_drawBreathHint", "863");
    addMapping("cg_draw2D", "2562");
    addMapping("cg_viewVehicleInfluence", "3019");
    addMapping("cg_fovMin", "361");
    addMapping("cg_hintFadeTime", "310");
    addMapping("cg_weaponHintsCoD1Style", "629");
    addMapping("cg_cursorHints", "4521");
    addMapping("cg_drawGun", "1762");
    addMapping("cg_fovScale", "3078");

    //3708 1 skips the title screen
    //4835 disabled game start button
    //4670 0 removes microtransaction deals from qm menu

    //4660 some render bool
}

void DvarInterface::init() {
#ifdef DEVELOPMENT_BUILD
    Console::initPrint(std::string(__FUNCTION__));
#endif // DEVELOPMENT_BUILD
    DvarInterface::addAllMappings();
    Console::infoPrint("Mapped " + std::to_string(DvarInterface::userToEngineMap.size()) + " DVars");
}