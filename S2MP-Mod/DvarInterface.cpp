////////////////////////////////////////////
//          DVar Interface
// 
//  Convert DVar strings into enumerated 
//  DVar strings for S2MP
////////////////////////////////////////////
#include "pch.h"
#include "DvarInterface.hpp"
#include "Console.h"
#include "GameUtil.hpp"
#include "FuncPointers.h"
#include <algorithm>

std::unordered_map<std::string, std::string> DvarInterface::userToEngineMap;
std::unordered_map<std::string, std::string> DvarInterface::engineToUserMap;

//Sets a Dvar in engine using the Dvar Interface
//Takes the name of the dvar, and the cmd vector
//Returns true if dvar was found and changed
bool DvarInterface::setDvar(std::string& dvarname, std::vector<std::string> cmd) {
    std::string dvarLower = dvarname;
    std::transform(dvarLower.begin(), dvarLower.end(), dvarLower.begin(), GameUtil::asciiToLower);

    dvar_t* var = Functions::_Dvar_FindVar(DvarInterface::toEngineString(dvarLower).c_str());
    //TODO: Add more dvar types support
    if (var != nullptr) {
        switch (var->type) {
        case DVAR_TYPE_BOOL:
            if (cmd.size() >= 2) {
                var->current.enabled = GameUtil::stringToBool(cmd[1]);
            }
            break;
        case DVAR_TYPE_FLOAT:
            if (cmd.size() >= 2) {
                var->current.value = GameUtil::safeStringToFloat(cmd[1]);
            }
            break;
        case DVAR_TYPE_INT:
            if (cmd.size() >= 2) {
                var->current.integer = GameUtil::safeStringToInt(cmd[1]);
            }
            break;
        case DVAR_TYPE_ENUM:
            if (cmd.size() >= 2) {
                var->current.integer = GameUtil::safeStringToInt(cmd[1]);
            }
            break;
        case DVAR_TYPE_STRING:
            if (cmd.size() >= 2) {
                var->current.string = _strdup(cmd[1].c_str());
            }
            break;
        default:
            Console::print("Unsupported dvar type: " + std::to_string(var->type));
            break;
        }
        return true;
    }
    else {
        Console::print("no dvar \"" + dvarname + "\"");
        return false;
    }
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
        Console::devPrint(userString + " ----> " + it->second);
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

void DvarInterface::addAllMappings() {
    //from R_RegisterDvars
    addMapping("r_disable_clutter", "1841");
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

}

void DvarInterface::init() {
    Console::initPrint("DvarInterface::init()");
    DvarInterface::addAllMappings();
    Console::infoPrint("Mapped " + std::to_string(DvarInterface::userToEngineMap.size()) + " DVars");
}