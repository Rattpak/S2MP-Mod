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
std::unordered_map<std::string, std::string> DvarInterface::descriptionMap; //so descriptions are no longer stored within the dvar_t structure :/

/**
 * @brief Sets a Dvar in the engine using the Dvar interface.
 *
 * Converts the provided user-facing dvar name to its engine equivalent,
 * builds a "set" command from the supplied command arguments, and
 * submits it to the engine command buffer.
 *
 * @param dvarname The user-facing name of the dvar to set.
 * @param cmd Command arguments where index 0 is the dvar name and
 *            subsequent elements are the values to assign.
 *
 * @return true if the dvar was found and a set command was issued;
 *         false if the dvar does not exist.
 */
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

/**
 * @brief Gets the number of registered user-to-engine dvar mappings.
 *
 * @return The number of dvar mappings currently stored.
 */
unsigned int DvarInterface::getDvarListSize() {
    return DvarInterface::userToEngineMap.size();
}

/**
 * @brief Adds a user-to-engine dvar name mapping.
 *
 * The user-facing string is normalized to lowercase before being stored.
 *
 * @param userString The user-facing dvar name.
 * @param engineString The engine-facing dvar name.
 */
void DvarInterface::addMapping(const std::string& userString, const std::string& engineString) {
    std::string dvarLower = userString;
    std::transform(dvarLower.begin(), dvarLower.end(), dvarLower.begin(), GameUtil::asciiToLower);
    userToEngineMap[dvarLower] = engineString;
    engineToUserMap[engineString] = userString;
}

/**
 * @brief Adds a user-to-engine dvar name mapping with a description.
 *
 * The user-facing string is normalized to lowercase before being stored.
 * The description is associated with the engine-facing dvar name.
 *
 * @param userString The user-facing dvar name.
 * @param engineString The engine-facing dvar name.
 * @param description A description of the dvar.
 */
void DvarInterface::addMapping(const std::string& userString, const std::string& engineString, const std::string& description) {
    std::string dvarLower = userString;
    std::transform(dvarLower.begin(), dvarLower.end(), dvarLower.begin(), GameUtil::asciiToLower);
    userToEngineMap[dvarLower] = engineString;
    engineToUserMap[engineString] = userString;
    
    //add desc
    descriptionMap[engineString] = description;
}


/**
 * @brief Retrieves the description for a dvar.
 *
 * @param engineStr The engine-facing dvar name.
 *
 * @return The dvar description if found; otherwise "undefined".
 */
std::string DvarInterface::getDvarDescription(const std::string& engineStr) {
    auto it = descriptionMap.find(engineStr);
    if (it != descriptionMap.end()) {
        return it->second;
    }
    return "undefined"; //couldnt find
}

/**
 * @brief Converts a user-facing dvar name to its engine-facing equivalent.
 *
 * The lookup is performed case-insensitively. If no mapping exists,
 * the original string is returned.
 *
 * @param userString The user-facing dvar name.
 *
 * @return The engine-facing dvar name, or the original string if no mapping exists.
 */
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

/**
 * @brief Converts an engine-facing dvar name to its user-facing equivalent.
 *
 * If no mapping exists, the original engine string is returned.
 *
 * @param engineString The engine-facing dvar name.
 *
 * @return The user-facing dvar name, or the original string if no mapping exists.
 */
std::string DvarInterface::toUserString(const std::string& engineString) {
    auto it = engineToUserMap.find(engineString);
    if (it != engineToUserMap.end()) {
        return it->second;
    }
    return engineString; //couldnt find
}


/**
 * @brief Registers a dvar using the same name for both user and engine mappings.
 *
 * @param name The dvar name.
 */
void DvarInterface::addDvarsWithName(const char* name) {
    addMapping(name, name);
}


/**
 * @brief Registers a boolean dvar and adds it to the dvar mapping system.
 *
 * @param name The name of the dvar.
 * @param val The default value.
 * @param flags Dvar flags.
 * @param description A description of the dvar.
 */
void DvarInterface::registerBool(const char* name, bool val, int flags, const char* description) {
    Functions::_Dvar_RegisterBool(name, val, flags);
    DvarInterface::addMapping(name, name, description); //TODO: make a dedicated function for custom dvars
}

void DvarInterface::addAllMappings() {
    //taking these from the generated user_config_mp.cfg
    //doing this since console pulls directly from dvar mapping
    //also some random ones i found
    addDvarsWithName("win_useWmInput");
    addDvarsWithName("com_maxfps");
    addDvarsWithName("com_graphicsTier");
    addDvarsWithName("com_setRecommendedPass");
    addDvarsWithName("ragdoll_enable");
    addDvarsWithName("r_lodScaleSkinned");
    addDvarsWithName("r_lodBiasSkinned");
    addDvarsWithName("r_lodScaleRigid");
    addDvarsWithName("r_lodBiasRigid");
    //addDvarsWithName("r_drawWater"); //doesnt do anything
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
    addDvarsWithName("vehCam_planeChaseOffset");
    addDvarsWithName("vehCam_splitscreenPlaneChaseOffset");
    addDvarsWithName("shadowplay_killswitch");
    addDvarsWithName("vehCam_chaseFD_ADSCamOffset");
    addDvarsWithName("vehCam_chaseFD_ADSVelocityCamOffsetMultBoostAcceleration");
    addDvarsWithName("vehCam_chaseFD_ADSVelocityCamOffsetMultAcceleration");
    addDvarsWithName("vehCam_chaseFD_ADSVelocityCamOffsetMultDeceleration");
    addDvarsWithName("vehCam_chaseFD_nonADSCamOffset");
    addDvarsWithName("vehCam_chaseFD_transitionSpawnCamOffset");
    addDvarsWithName("vehCam_chaseFD_transitionSpawnCamAngleOffset");
    addDvarsWithName("vehCam_chaseFD_normalSpawnCamOffset");
    addDvarsWithName("vehCam_chaseFD_nonADSVelocityCamOffsetMultAcceleration");
    addDvarsWithName("vehCam_chaseFD_nonADSVelocityCamOffsetMultDeceleration");
    addDvarsWithName("telemetry_error_killswitch");
    addDvarsWithName("scr_thirdPerson");
    addDvarsWithName("cg_hudObjectiveTextScale");
    addDvarsWithName("scr_game_graceperiod");
    addDvarsWithName("scr_game_playerwaittime");
    addDvarsWithName("scr_game_matchstarttime");
    addDvarsWithName("scr_game_roundstarttime");
    addDvarsWithName("scr_game_allowkillcam");
    addDvarsWithName("scr_game_onlyheadshots");
    addDvarsWithName("scr_game_deathpointloss");
    addDvarsWithName("scr_game_suicidepointloss");
    addDvarsWithName("scr_game_suicidespawndelay");
    addDvarsWithName("scr_game_spectatetype");
    addDvarsWithName("scr_game_lockspectatorpov");
    addDvarsWithName("scr_game_perks");
    addDvarsWithName("scr_game_forceuav");
    addDvarsWithName("scr_game_compassRadarUpdateTime");
    addDvarsWithName("scr_game_radarMode");
    addDvarsWithName("scr_game_hardpoints");
    addDvarsWithName("scr_game_killstreakdelay");
    addDvarsWithName("scr_game_minimapHiddenWhileADS");
    addDvarsWithName("scr_oneShot");
    addDvarsWithName("scr_wanderlustOnly");
    addDvarsWithName("leprechauns_enabled");
    addDvarsWithName("spv_ground_war_active");
    addDvarsWithName("sv_blacklistReasons");
    addDvarsWithName("scr_hardcore_StreaksTeamKill");
    addDvarsWithName("scr_enable_flybywire");
    addDvarsWithName("scr_hardpoint_allowartillery");
    addDvarsWithName("scr_hardpoint_allowuav");
    addDvarsWithName("scr_hardpoint_allowhelicopter");
    addDvarsWithName("scr_team_fftype");
    addDvarsWithName("scr_team_respawntime");
    addDvarsWithName("scr_team_teamkillpointloss");
    addDvarsWithName("scr_team_teamkillspawndelay");
    addDvarsWithName("scr_team_teamkillkicklimit");
    addDvarsWithName("scr_player_numlives");
    addDvarsWithName("scr_player_respawndelay");
    addDvarsWithName("scr_player_maxhealth");
    addDvarsWithName("scr_player_suicidespawndelay");
    addDvarsWithName("scr_player_healthregentime");
    addDvarsWithName("scr_player_forcerespawn");
    addDvarsWithName("scr_player_sprinttime");
    addDvarsWithName("isMLGMatch");
    addDvarsWithName("isEsportsMatch");
    addDvarsWithName("spawning_revised_domination");
    addDvarsWithName("spawning_revised_teamdeathmatch");
    addDvarsWithName("spawning_revised_capturetheflag");
    addDvarsWithName("spawning_revised_hardpoint");
    addDvarsWithName("spawning_revised_cornersighttraces");
    addDvarsWithName("spawning_revised_frontline");
    addDvarsWithName("spawning_revised_fatique");
    addDvarsWithName("spawning_use_classic");
    addDvarsWithName("scr_hardcore");
    addDvarsWithName("scr_diehard");
    addDvarsWithName("scr_oldschool");
    addDvarsWithName("ui_hud_obituaries");


    addMapping("g_gametype", "1924", "The current game mode");
    addMapping("nextmap", "4059", "Next map to play");
    addMapping("mapname", "1673");
    //from R_RegisterDvars
    addMapping("r_texFilterDisable", "91", "Disables all texture filtering (uses nearest only.)");
    addMapping("r_characterSceneEnable", "5475");
    addMapping("r_texFilterMipMode", "4107", "Forces all mipmaps to use a particular blend between levels (or disables mipping.)");
    addMapping("r_texShowMipMode", "2416", "Forces textures with the specified mip filtering to draw black.");
    addMapping("r_texFilterMipBias", "3569", "Change the mipmap bias");
    addMapping("r_texFilterProbeBilinear", "5634", "Force reflection probe to use bilinear filter");
    addMapping("r_lodDynamicScale", "1111");
    addMapping("r_artUseTweaks", "5960");
    addMapping("r_lightGridTempSmoothingFactor", "2677");
    addMapping("r_globalSecondarySelfVisScale", "1264");
    addMapping("r_globalSecondarySelfVisLerpToFullyOpen", "5139");
    addMapping("r_lightMap", "3271", "Replace all lightmaps with pure black or pure white");
    addMapping("r_colorMap", "3391", "Replace all color maps with pure black or pure white");
    addMapping("r_detailMap", "2194", "Replace all detail maps with an image that effectively disables them");
    addMapping("r_displacementMap", "4388", "Replace all displacement maps with an image that effectively disables them");
    addMapping("r_normalMap", "5467", "Replace all normal maps with a flat normal map");
    addMapping("r_specularMap", "2427", "Replace all specular maps with pure black (off) or pure white (super shiny)");
    addMapping("r_specOccMap", "5620", "Replace all specular occlusion maps with pure black (fully occluded) or pure white (not occluded)");
    addMapping("r_envBrdfLutMap", "3818", "Replace environment BRDF lookup table with pure black (no secondary specular) or pure white (maximum secondary specular)");
    addMapping("r_emissiveMap", "769", "Replace all emissive maps with pure black or pure white");
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
    addMapping("sm_strictCull", "4889", "Strict shadow map cull");
    addMapping("sm_spotDistCull", "1875", "Distance cull spot shadows");
    addMapping("sm_fastSunShadow", "4583", "Fast sun shadow");
    addMapping("sm_debugFastSunShadow", "4583", "Debug fast sun shadow");
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
    addMapping("r_outdoorFeather", "161", "Outdoor z-feathering value");
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
    addMapping("r_patchCountAllowed", "3925", "Enable run-time setting of patch count per draw call.");
    addMapping("r_subdivPatchCount", "4902", "Patches per thread group for sub-division surfaces.");
    addMapping("r_displacementPatchCount", "3287", "Patches per thread group for displacement surfaces.");
    addMapping("r_defaultPatchCount", "3923", "Patches per thread group for all other surfaces.");
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
    addMapping("r_preBakedTndTweaks", "297");
    addMapping("r_delayAddSceneModels", "3337");
    addMapping("r_emblemBrightnessScale", "2735");
    addMapping("r_balanceLightmapOpaqueLists", "2599", "Split lightmap opaque into multiple draw lists.");
    addMapping("r_volumeLightScatterUseTweaks", "3889");
    addMapping("r_volumeLightScatterLinearAtten", "2564");
    addMapping("r_volumeLightScatterQuadraticAtten", "3758");
    addMapping("r_volumeLightScatterAngularAtten", "750");
    addMapping("r_volumeLightScatterDepthAttenNear", "3070");
    addMapping("r_volumeLightScatterDepthAttenFar", "5444");
    addMapping("r_volumeLightScatterBackgroundDistance", "4506");
    addMapping("r_volumeLightScatterColor", "3200");
    addMapping("r_volumeLightScatterEv", "4638");
    //addMapping("r_surfaceHDRScalarUseTweaks", "5058");
    addMapping("r_unlitSurfaceHDRScalar", "284", "Vision set based scalar applied to unlit surfaces to balance those surfaces with the luminance of the scene");
    addMapping("r_litSurfaceHDRScalar", "1097", "Vision set based scalar applied to lit surfaces");
    addMapping("r_ssaoUseTweaks", "4822");
    addMapping("r_ssaoWidth", "919");
    addMapping("r_ssaoDepthScale", "1870");
    addMapping("r_ssaoDepthScaleViewModel", "2358");
    addMapping("r_ssaoGapFalloff", "1869");
    addMapping("r_ssaoGradientFalloff", "4816");
    addMapping("r_ssaoFadeDepth", "3404");
    addMapping("r_ssaoRejectDepth", "626");
    addMapping("r_ssaoMinPixelWidth", "225");
    addMapping("r_ssaoScriptScale", "212");
    addMapping("r_ssaoStrength", "4361");
    addMapping("r_ssaoPower", "3537");
    addMapping("r_aoUseTweaks", "5618");
    addMapping("r_aoStrength", "3697");
    addMapping("r_aoPower", "3888");
    addMapping("r_aoDiminish", "5848");
    addMapping("r_aoBlurSharpness", "609");
    addMapping("r_aoBlurStep", "3283");
    addMapping("r_hemiAoMaxDepthDownsample", "2983");
    addMapping("r_hemiAoQualityLevel", "1044");
    addMapping("r_hemiAoNoiseFilterTolerance", "487");
    addMapping("r_hemiAoBlurTolerance", "2117");
    addMapping("r_hemiAoUpsampleTolerance", "2689");
    addMapping("r_hemiAoRejectionFalloff", "4830");
    addMapping("r_hemiAoCombineResolutionsBeforeBlur", "465");
    addMapping("r_hemiAoCombineResolutionsWithMul", "3011");
    addMapping("r_hemiAoHierarchyDepth", "5329");
    addMapping("r_hemiAoDepthSquash", "2843");
    addMapping("r_hemiAoStrength", "554", "Strength of Hemi Screen Space Ambient Occlusion effect");
    addMapping("r_hemiAoPower", "3869");
    addMapping("r_gtaoQualityLevel", "5156");
    addMapping("r_cacheModelLighting", "4636");
    addMapping("r_showModelLightingLowWaterMark", "3116");
    addMapping("r_keepSunShadowCache", "1708");
    addMapping("r_blur", "797", "Dev tweak to blur the screen");
    addMapping("r_distortion", "5094", "Enable distortion");
    addMapping("r_distortion_script_force_off", "922", "Force distortion off in script");
    addMapping("r_globalGenericMaterialScale", "2807", "Hack global generic material constants");
    addMapping("r_filmTweakLightTint", "4601", "Tweak dev var; film color light tint color");
    addMapping("r_filmTweakMediumTint", "905", "Tweak dev var; film color medium tint color");
    addMapping("r_filmTweakDarkTint", "1760", "Tweak dev var; film color dark tint color");
    addMapping("r_filmTweakInvert", "3026", "Tweak dev var; enable inverted video");
    addMapping("r_filmTweakDesaturationDark", "724", "Tweak dev var; Additional desaturation applied after all 3D drawing to dark areas");
    addMapping("r_filmTweakDesaturation", "3859", "Tweak dev var; Desaturation applied after all 3D drawing to light");
    addMapping("r_filmTweakBrightness", "2750", "Tweak dev var; film color brightness");
    addMapping("r_filmTweakEnable", "3114", "Tweak dev var; enable film color effects");
    addMapping("r_filmTweakContrast", "2440", "Tweak dev var; film color contrast");
    addMapping("r_filmTweakIntensity", "3025", "Tweak dev var; film color intensity");
    addMapping("r_brightness", "1691", "Brightness adjustment");
    addMapping("r_desaturation", "1821", "Desaturation adjustment");
    addMapping("r_loadForRenderer", "4838", "Set to false to disable dx allocations (for dedicated server mode)");

    //from CG_CompassRegisterDvars
    addMapping("compassSize", "2692", "Scale the compass");
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
    addMapping("cg_hudMapRadarLineThickness", "4365", "Thickness, relative to the map width, of the radar texture that sweeps across the full screen map");
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
    addMapping("theater_active", "616", "Are we allowed to show theater or not.");
    addMapping("facebook_active", "4761", "Are we allowed to show Facebook or not");
    addMapping("facebook_delay", "1636", "Delay before the Facebook calls start to Demonware. -1 means On-Demand and it will wait until the 'startfacebook' menu call");
    addMapping("facebook_max_retry_time", "5797", "Max time that the Facebook authentication can retry");
    addMapping("facebook_retry_step", "4142", "Step in m/s for the Facebook authentication retry");
    addMapping("facebook_friends_max_retry_time", "339");
    addMapping("facebook_friends_retry_step", "2306");
    addMapping("facebook_friends_refresh_time", "5238");
    addMapping("facebook_friends_throttle_time", "5232");
    addMapping("facebook_friends_active", "2687");
    addMapping("facebook_upload_video_active", "2688");
    addMapping("facebook_upload_photo_active", "5141");
    addMapping("userGroup_active", "1021", "Are we allowed to show Usergroups or not");
    addMapping("userGroup_max_retry_time", "1490");
    addMapping("userGroup_retry_step", "1419");
    addMapping("userGroup_RetryTime", "227");
    addMapping("userGroup_refresh_time_secs", "55");
    addMapping("userGroup_cool_off_time", "776");
    addMapping("elite_clan_delay", "2672", "Delay before the bdTeams calls start to Demonware. -1 means On-Demand and it will wait until the 'starteliteclan' menu call");
    addMapping("elite_clan_active", "3629", "Are we allowed to show Elite Clans or not");
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
    addMapping("dw_presence_active", "4174", "Is the demonware presence system enabled");
    addMapping("dw_presence_coop_join", "418", "Do we allow players to join on presence for private coop matches (post session to demonware)");
    addMapping("dw_presence_put_delay", "3550", "Number of milliseconds to wait in a presence state before sending to demonware");
    addMapping("dw_presence_put_rate", "561", "Number of milliseconds to wait between sending presence state to demonware");
    addMapping("dw_presence_get_rate", "3602", "Number of milliseconds to wait between fetching presence state from demonware");
    addMapping("dw_interleave_all_nat_types", "3358");
    addMapping("dw_enable_connection_telemetry", "1799");
    addMapping("dw_nattrav_cache_enable", "770");
    addMapping("dw_nattrav_cache_timeout", "463");
    addMapping("num_available_map_packs", "5942", "Number of map packs available for this platform");
    addMapping("clientNetPerf_enabled", "1427");
    addMapping("clientNetPerf_UserCmdTimeWindowMs", "156");
    addMapping("clientNetPerf_UserCmdProcessedMinCount", "5040");
    addMapping("clientNetPerf_UserCmdQueuedMinCount", "4912");
    addMapping("clientNetPerf_UserCmdDroppedMinCount", "2614");
    addMapping("sv_clientPacketsBurstMinCount", "2044");
    addMapping("iotd_active", "2623", "Is the IOTD system enabled");
    addMapping("iotd_retry", "60", "Can the IOTD system retry fetching data from Demonware");
    addMapping("igs_td", "532", "Show Trial DLC");
    addMapping("matchdata_active", "967", "Are match data uploads enabled");
    addMapping("matchdata_maxcompressionbuffer", "2660", "Max SP match data compression buffer to use (in bytes)");
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
    addMapping("match_making_telemetry_chance", "743", "The % chance of sending match making telemetry");
    addMapping("log_host_migration_chance", "2574", "The % chance of host migration results telemetry");
    addMapping("max_ping_threshold_good", "4332", "max ping value to be considered as good");
    addMapping("max_ping_threshold_medium", "4760", "max ping value to be considered as medium");
    addMapping("aci", "160", "anticheat infraction");
    addMapping("vpte", "5029");
    addMapping("zombiesAllowSoloPause", "5353");
    addMapping("dlog_active", "5785");
    addMapping("marketing_refresh_time", "1543", "time in seconds to wait before refreshing marketing messages from demonware");
    addMapping("emblems_active", "4798", "Are we allowed to enable Emblems or not");
    addMapping("selfie_active", "387", "Are we allowed to enable Selfie Uploads or not");
    addMapping("ca_intra_only", "3302", "CoD Anywhere Intra Network Only");
    addMapping("ca_do_mlc", "1966", "CoD Anywhere Do Multi Login check");
    addMapping("ca_require_signin", "2520", "CoD Anywhere require sign in to enter MP");
    addMapping("ca_auto_signin", "5570", "CoD Anywhere start sign-in task automatically on startup or first party sign-in");
    addMapping("lb_times_in_window", "3355", "Lobby throttling window amount");
    addMapping("lb_window", "2163", "Lobby throttling window");
    addMapping("svwp", "765", "playerdata server write protection: 0 = disable, 1 = silent, 2 = kick");
    addMapping("dc_lobbymerge", "3583", "Allows lobby merging across data centres");
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
    addMapping("lui_menuFlowEnabled", "4436", "Enables LUI menu flow");
    addMapping("lui_xboxlive_menu", "3547", "Enables the LUI xboxlive menu");
    addMapping("lui_systemlink_menu", "1572", "Enables the LUI systemlink menu");
    addMapping("lui_splitscreenupscaling", "659", "Force splitscreen upscaling off/on (-1 off, 1 on) -- requires map change");
    addMapping("lui_draw_hints", "5270");
    addMapping("e3demo", "2803");
    addMapping("e3demo_host", "4605");
    addMapping("e3demo_client", "1303");
    addMapping("e3demo_show_client_title_screen", "871");
    addMapping("lui_hud_motion_enabled", "42", "Enable hud motion");
    addMapping("lui_hud_motion_perspective", "5345", "value for hud motion perspective transform in pixels");
    addMapping("lui_hud_motion_translation_scale", "381", "lui_hud_motion_translation_scale");
    addMapping("lui_hud_motion_translation_max", "4686", "Hud motion translation max");
    addMapping("lui_hud_motion_rotation_scale", "386", "Hud motion rotation scale");
    addMapping("lui_hud_motion_rotation_max", "1033", "Hud motion rotation max");
    addMapping("lui_hud_motion_bob_scale", "5406", "Hud motion bob scale");
    addMapping("lui_hud_motion_angle_ease_speed", "2690", "Hud motion ease percentage of degrees per second");
    addMapping("lui_hud_motion_trans_ease_speed", "397", "Hud motion ease percentage of pixels per second");
    addMapping("lui_FFotDSupportEnabled", "4373", "Enables lui to update itself via the ffotd");
    addMapping("lui_disable_blur", "262", "Disable LUI blur"); //DOUBLE CHECK THIS
    addMapping("dev_auto_hubstart", "3708", "Auto load the game into the MP hub");
    addMapping("hub_vendor_overhead_min_distance", "1647");
    addMapping("hub_vendor_overhead_max_distance", "3924");
    addMapping("hub_supply_drop_max_distance", "4143");
    addMapping("hub_leaderboard_max_distance", "3296");
    addMapping("LUI_MemErrorsFatal", "1626", "Out of memory errors cause drops when true, reinits the UI system if false");

    addMapping("cg_foliagesnd_alias", "4011", "The sound that plays when an actor or player enters a foliage clip brush.");
    addMapping("cg_broadcasterSkycamDistance", "3119");
    addMapping("cg_subtitleForcedColor", "5004");
    addMapping("cg_subtitleColor", "2191");
    addMapping("cg_gunReticleTeamColor_EnemyTeam", "4426");
    addMapping("cg_gunReticleTeamColor_MyTeam", "4893");
    addMapping("cg_ScorestreakColor_Enemy", "2906");
    addMapping("cg_weaponVisInterval", "412", "Do weapon vis checks once per this many frames, per centity");
    addMapping("cg_disableScreenShake", "3926", "Turns off screen shakes when turned on. Dev-only");
    addMapping("useRelativeTeamColors", "1244");
    addMapping("cg_weapHitCullEnable", "5446", "When true, cull back facing weapon hit fx.");
    addMapping("cg_weapHitCullAngle", "207", "Angle of cone within which to cull back facing weapon hit effects");
    addMapping("overrideNVGModelWithKnife", "5025", "When true, nightvision animations will attach the weapDef's knife model instead of the night vision goggles.");
    addMapping("cg_viewZSmoothingTime", "957", "Amount of time to spread the smoothing over");
    addMapping("cg_viewZSmoothingMax", "2391", "Threshhold for the maximum smoothing distance we'll do");
    addMapping("cg_viewZSmoothingMin", "5640", "Threshhold for the minimum smoothing distance it must move to smooth");
    addMapping("cg_invalidCmdHintBlinkInterval", "811", "Blink rate of an invalid command hint");
    addMapping("cg_invalidCmdHintDuration", "641", "Duration of an invalid command hint");
    addMapping("cg_flashbangNameFadeOut", "3428", "Time in milliseconds to fade out friendly names when flash banged");
    addMapping("cg_flashbangNameFadeIn", "5860", "Time in milliseconds to fade in friendly names after a flashbang");
    addMapping("cg_friendlyNameFadeOut", "5344", "Time in milliseconds to fade out friendly names");
    addMapping("cg_friendlyNameFadeIn", "3768", "Time in milliseconds to fade in friendly names");
    addMapping("cg_drawFriendlyNames", "1380", "Whether to show friendly names in game");
    addMapping("cg_overheadNamesFont", "731", "Font for overhead names");
    addMapping("cg_overheadNamesGlow", "257", "Glow color for overhead names");
    addMapping("cg_overheadNamesFarScale", "1900", "The amount to scale overhead name sizes at cg_overheadNamesFarDis");
    addMapping("cg_overheadNamesFarDist", "2393", "The far distance at which name sizes are scaled by cg_overheadNamesFarScale");
    addMapping("cg_overheadNamesNearDist", "1733", "The near distance at which names are full size");

    addMapping("com_errorResolveCommand", "4278", "Command to run when they close the error box");
    
    addMapping("r_portalMinClipArea", "634", "Don't clip child portals by a parent portal smaller than this fraction of the screen area.");
    addMapping("r_portalMinRecurseDepth", "3100", "Ignore r_portalMinClipArea for portals with fewer than this many parent portals.");
    addMapping("r_sunshadowmap_cmdbuf_worker", "5260", "Process shadowmap command buffer in a separate thread");
    addMapping("r_animatedVertsUseNoCacheLimit", "2274");
    addMapping("r_animatedVertsNoCacheScale", "3220");
    addMapping("r_screenSpaceShadows", "4712");
    addMapping("r_maxScreenSpaceShadowsSamplesTotal", "4241");
    addMapping("r_minScreenSpaceShadowsSamplesPerLight", "4941");
    addMapping("r_volumeLightScatter", "2696", "Enables volumetric light scattering");
    addMapping("r_useLightGridDefaultModelLightingLookup", "2706", "Enable/disable default model lighting lookup");
    addMapping("r_lightGridDefaultModelLightingLookup", "3460", "Default model lighting lookup location");
    addMapping("r_useLightGridDefaultFXLightingLookup", "1074", "Enable/disable default fx lighting lookup");
    addMapping("r_lightGridDefaultFXLightingLookup", "1307", "Default FX lighting lookup location");
    addMapping("r_blurdstGaussianBlurRadius", "190", "Amount to gaussian blur blur distortion render target");
    addMapping("r_blurdstGaussianBlurLevel", "3261", "MIP level to start gaussian blur at");
    addMapping("r_uiBlurDstMode", "1175", "UI blur distortion mode. Fast uses the scene mip map render target, PostSun uses a downsampled post sun resolve buffer, PostSun HQ uses a gaussian blurred post sun resolve buffer.");
    addMapping("sm_spotLightScoreModelScale", "4230", "Scale the calculated spot light score by this value if the light currently only affects static or script brush models.");
    addMapping("sm_minSpotLightScore", "5176", "Minimum score (based on intensity, radius, and position relative to the camera) for a spot light to have shadow maps.");
    addMapping("sm_spotShadowFadeTime", "3286", "How many seconds it takes for a primary light shadow map to fade in or out");
    addMapping("r_useShadowGeomOpt", "4163", "Enable iwRad shadow geometry optimization. It only works when we have the data generated in iwRad.");
    addMapping("r_fog_depthhack_scale", "645", "Fog scale for depth hack surfaces");
    addMapping("r_materialWind", "1520");
    addMapping("r_mpRimDiffuseTint", "817", "Change character's rim diffuse tint for multiplayer.");
    addMapping("r_mpRimStrength", "2634", "Change character's rim strength for multiplayer");
    addMapping("r_mpRimColor", "2013", "Change character's rim color for multiplayer");
    addMapping("r_hudFx", "4475", "Draw HUD Effects");
    //addMapping("r_fog", "3425", "Set to 0 to disable fog"); //might just use custom one
    addMapping("r_polygonOffsetScale", "3557", "Offset scale for decal polygons; bigger values z-fight less but poke through walls more");
    addMapping("r_zfar", "484", "Change the distance at which culling fog reaches 100% opacity; 0 is off");
    addMapping("r_znear", "3153");
    addMapping("r_subwindow", "4019");

    //FX_RegisterDvars
    addMapping("fx_enable", "3917", "Toggles all effects processing");
    addMapping("fx_lightgridplus_enable", "5637");
    addMapping("fx_draw", "3461", "Toggles drawing of effects after processing");
    addMapping("fx_draw_spotLight", "2157", "Toggles drawing of effects after processing");
    addMapping("fx_draw_omniLight", "5408", "Toggles drawing of effects after processing");
    addMapping("fx_visMinTraceDist", "3344", "Minimum visibility trace size");
    addMapping("fx_drawClouds", "1398", "Toggles the drawing of particle clouds");
    addMapping("fx_deferelem", "452", "Toggles deferred processing of elements instead of effects");
    addMapping("fx_killEffectOnRewind", "1666", "Causes effects that have been marked for a soft kill (fade out) to be killed immediately on a rewind.");
    addMapping("fx_lightmap_pixels_per_texel", "3085");
    addMapping("fx_lightmap_max_level", "557");
    addMapping("fx_alphaThreshold", "3541", "Don't draw billboard sprites, oriented sprites or tails with alpha below this threshold (0-256).");
    addMapping("fx_use_rewind_flags", "4994");
    addMapping("fx_physicsImpactVelocityThreshold", "2224", "Set the min normal velocity threshold in order for model physics fx to generate child impact effects.");
    addMapping("fx_cast_shadow", "2527", "Enable transparency shadow mapping from script");
    addMapping("fx_lightGridSampleOffset", "4525", "the length of effect sample's offset along X Axis");
    addMapping("glass_break", "2210", "Toggle whether or not glass breaks when shot");
    addMapping("glass_edge_angle", "2449", "Sets the range of angle deflections used by new glass pieces on a supported edge");
    addMapping("glass_fall_ratio", "3991", "Ratio of piece area to supporting edge length squared.  Below the min, the piece never falls.");
    addMapping("glass_fall_delay", "474", "glass_fall_delay");
    addMapping("glass_fx_chance", "2355", "Chance to play an effect on a small piece of glass when it hits the ground");
    addMapping("glass_hinge_friction", "2848", "Friction used by moving glass pieces when joined like a hinge to a frame");
    addMapping("glass_max_pieces_per_frame", "3251", "Maximum number of pieces to create in one frame. This is a guideline and not a hard limit.");
    addMapping("glass_max_shatter_fx_per_frame", "3958", "Maximum number of shatter effects to play in one frame This is a guideline and not a hard limit.");
    addMapping("glass_shard_maxsize", "3595", "The maximum area for a flying piece of glass when shattering. Pieces larger than this will be broken into smaller ones");
    addMapping("glass_fringe_maxsize", "5363", "The maximum area for an edge piece of glass when shattering. Pieces larger than this will be broken into smaller ones");
    addMapping("glass_fringe_maxcoverage", "4886", "The maximum portion of the original piece of glass that is allowed to remain after the glass shatters");
    addMapping("glass_trace_interval", "2239", "The length of time, in milliseconds, between glass piece traces");
    addMapping("glass_physics_chance", "3430", "The chance for a given shard of glass to use physics");
    addMapping("glass_physics_maxdist", "436", "The maximum distance of a glass piece from the player to do physics");
    addMapping("glass_shattered_scale", "1567", "The scale of the shattered glass material");
    addMapping("glass_crack_pattern_scale", "4887", "The scale applied to the radius used for the crack pattern");
    addMapping("glass_fall_gravity", "2436", "Gravity for falling pieces of glass");
    addMapping("glass_linear_vel", "3330", "Sets the range of linear velocities used by new glass pieces");
    addMapping("glass_angular_vel", "3766", "Sets the range of angular velocities used by new glass pieces");
    addMapping("fx_dynamicGritDisableBlood", "543");
    addMapping("fx_dynamicGritDisableHide", "5442");
    addMapping("fx_dynamicGritDisableFire", "1487");

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
    addMapping("LUI_WorkerCmdGC", "3886", "Dev-only flag to enable/disable LUI workerCmd GC thread");

    //ComScore_Init
    addMapping("comscore_backoff", "3219", "constants for the comscore backoff function a*(x-b)^2+c");

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
    addMapping("cl_paused", "183", "Pause the game");
    addMapping("cg_teamChatsOnly", "1127", "Allow chatting only on the same team");
    addMapping("cg_chatHeight", "2662", "The font height of a chat message");
    addMapping("cg_chatTime", "804", "The amount of time that a chat message is visible");
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
    addMapping("cg_errordecay", "4391", "Decay for predicted error");
    addMapping("cg_processImmediateEvents", "2921");
    addMapping("cg_tweenExtrapolationPeriodMs", "5173");
    addMapping("cg_tweenOverrideThresholdMs", "4088");
    addMapping("cg_tweenOverridePeriodSlowMs", "2778");
    addMapping("cg_tweenOverridePeriodMs", "1906");
    addMapping("cg_brass", "3385", "Weapons eject brass");
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
    addMapping("cg_drawMantleHint", "2104", "Draw a 'press key to mantle' hint");
    addMapping("cg_drawDoubleTapDetonateHint", "1784", "Draw a 'double tap to detonate grenade' hint");
    addMapping("cg_drawBreathHUD", "1668");
    addMapping("cg_drawBreathHint", "863", "Draw a 'hold breath to steady' hint");
    addMapping("cg_draw2D", "2562", "Draw 2D screen elements");
    addMapping("cg_viewVehicleInfluence", "3019", "The influence on the view angles from being in a vehicle");
    addMapping("cg_fovMin", "361", "The minimum possible field of view");
    addMapping("cg_hintFadeTime", "310", "Time in milliseconds for the cursor hint to fade");
    addMapping("cg_weaponHintsCoD1Style", "629", "Draw weapon hints in CoD1 style: with the weapon name, and with the icon below");
    addMapping("cg_cursorHints", "4521", "Draw cursor hints where: 0: no hints 1 : sin size pulse 2 : one way size pulse 3 : alpha pulse 4 : static image");
    addMapping("cg_drawGun", "1762", "Draw the view model");
    addMapping("cg_fovScale", "3078", "Scale applied to the field of view");

    //filesystem
    addMapping("fs_basepath", "4972", "Base game path");
    addMapping("fs_basegame", "2796", "Base game name");
    addMapping("fs_game", "1751", "Game data directory. Must be \"\" or a sub directory of 'mods/'.");
    addMapping("fs_ignoreLocalized", "3139", "Ignore localized files");
    addMapping("fs_homepath", "4068", "Game home path");
    addMapping("fs_debug", "1467", "Enable file system debugging information"); //not used in engine but still exists. I will use it tho

    //CG_ViewRegisterDvars (not done)
    addMapping("cg_heliKillCamFov", "37", "Helicopter kill camera field of view.");
    addMapping("cg_heliKillCamNearBlur", "1863");
    addMapping("cg_heliKillCamFarBlur", "693");
    addMapping("cg_heliKillCamFarBlurStart", "5721");
    addMapping("cg_heliKillCamFarBlurDist", "5782");
    addMapping("cg_heliKillCamNearBlurStart", "3000");
    addMapping("cg_heliKillCamNearBlurEnd", "5460");
    addMapping("cg_heliKillCamFstop", "1136");
    addMapping("cg_airstrikeKillCamFov", "3720");
    addMapping("cg_airstrikeKillCamNearBlur", "3706");
    addMapping("cg_airstrikeKillCamFarBlur", "3822");
    addMapping("cg_airstrikeKillCamFarBlurStart", "5493");
    addMapping("cg_airstrikeKillCamFarBlurDist", "4446");
    addMapping("cg_airstrikeKillCamNearBlurStart", "5840");
    addMapping("cg_airstrikeKillCamNearBlurEnd", "5095");
    addMapping("cg_airstrikeCamFstop", "3160");
    addMapping("cg_explosiveKillCamUpDist", "1321");
    addMapping("cg_explosiveKillCamBackDist", "3692");

    addMapping("cg_remoteCameraZNear", "4317");
    addMapping("radarjamDistMin", "5504");
    addMapping("radarjamDistMax", "4545");
    addMapping("radarjamSinCurve", "4632");
    addMapping("thermalBlurFactorNoScope", "2346");
    addMapping("cg_cameraShakesAffectVehicleCamera", "3268");
    addMapping("cg_vehicleVMFovScaling", "2328");
    addMapping("cg_viewAnglePitchScale", "4877");
    addMapping("cg_hubTransitionLerpTime", "3993");
    addMapping("cg_hubZoomDoFfStop", "652");
    addMapping("cg_hubZoomDoFDist", "1108");
    addMapping("cg_hubZoomLerpTime", "2596");
    addMapping("cg_hubZoomFov", "4444");
    addMapping("cg_hubZoomDist", "861");

    addMapping("veh_boneControllerLodDist", "3878", "Distance at which bone controllers are not updated.");
    addMapping("vehCam_freeLook", "1293");
    addMapping("vehAudio_inAirPitchUpLerp", "1120");
    addMapping("vehAudio_inAirPitchDownLerp", "2350");
    addMapping("vehAudio_spawnVolumeTime", "141");
    

    //ui dvars ig
    addMapping("ui_netSource", "1639", "The network source where:   0:Local   1:Internet   2:Favorites");
    addMapping("ui_currentMap", "788", "Current map index");
    addMapping("ui_onlineRequired", "3556", "UI requires online connection to be present.");
    addMapping("ui_currentFeederMapIndex", "864", "Currently selected map");
    addMapping("ui_selectedFeederMap", "264", "Current preview game type");
    addMapping("ui_missingMapName", "2958", "Name of map to show in missing content error");
    addMapping("ui_serverStatusTimeOut", "2382", "Time in milliseconds before a server status request times out");
    addMapping("ui_buildLocation", "549", "Where to draw the build number");

    //WSA
    addMapping("dlog_devpointHost", "5138");

    //Server Demo Dvars
    addMapping("g_knockback", "4541");
    addMapping("g_maxDroppedWeapons", "1605", "Maximum number of dropped weapons");
    addMapping("g_inactivity", "2916", "Time delay before player is kicked for inactivity");
    //TODO: finish this later

    //4835 disabled game start button
    //4670 0 removes microtransaction deals from qm menu
    //4660 some render bool
}

void DvarInterface::init() {
    DEV_INIT_PRINT();
    DvarInterface::addAllMappings();
    Console::infoPrint("Mapped " + std::to_string(DvarInterface::userToEngineMap.size()) + " DVars");
}