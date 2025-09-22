#include "pch.h"
#include "structs.h"
#include "Console.hpp"
#include "FuncPointers.h"
#include <MinHook.h>
#include "GameUtil.hpp"
#include "DevDef.h"
#include "DvarInterface.hpp"

typedef void (*R_EndFrame_t)(void);
R_EndFrame_t _EndFrame = nullptr;

typedef void(*CL_KeyEvent)(int client, int key, int down);
CL_KeyEvent _CL_KeyEvent = nullptr;
font_t* InternalConsole::consoleFont = nullptr;
material_t* mtl_white = nullptr;

std::vector<std::string> outputStack;
std::vector<int> outputLogLevel;
int outputStackSeekPos = 0;
int maxLines = 0;
int lineSpacing = 16;

//Main Internal Console
float colorSecondary[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float colorPrimary[4] = { 0.01f, 0.01f, 0.01f, 1.0f };
float colorTextLabel[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float colorWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

float devBuildInfoCol[4] = { 1.0f, 1.0f, 1.0f, 0.2f };
float buildInfoCol[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

//Full Internal Console
float fullConBuildVerCol[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
float scrollbarTrackColorSecondary[4] = { 0.71f, 0.71f, 0.66f, 1.0f };
float scrollbarTrackColorPrimary[4] = { 0.58f, 0.58f, 0.55f, 1.0f };
float scrollbarSliderColorSecondary[4] = { 0.01f, 0.01f, 0.01f, 1.0f };
float scrollbarSliderColorPrimary[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
float warningColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
float errorColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float devColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
float colorFullSecondary[4] = { colorSecondary[0], colorSecondary[1], colorSecondary[2], 1.0f };
float colorFullPrimary[4] = { colorPrimary[0], colorPrimary[1], colorPrimary[2], 0.85f };

//Internal Console Command Stack
std::vector<std::string> cmdStack;
int cmdStackSeekPos = -1;
bool isCmdStackSeeking = false;

bool consoleOpen = false;
bool fullConsole = false;
float margin = 15.0f;
float consoleHeight = 25.0f;
float borderSize = 1.0f;
float fullConsoleGap = 4.0f;
std::string textBuffer = "";
int cursorPos = 0;

//searchbox vars
float hintBoxColorSecondary[4] = { colorSecondary[0], colorSecondary[1], colorSecondary[2], 1.0f};
float hintBoxColorPrimary[4] = { colorPrimary[0], colorPrimary[1],colorPrimary[2], 1.0f};
int lastSearchLength = -1;
std::vector<const dvar_t*> searchResults;
int MAX_SEARCH_RESULTS = 24; //was a const a swear
int resultCount = 0;
int searchBoxOffset = 96;
int textStartOffsetGapY = 5;
int valueDisplayOffsetX = 350;
float commandColor[4] = { 0.584f, 0.584f, 0.839f, 1.0f };
std::vector<dvar_t*> cDvars;
bool stopSearch = false; //v1.2.8

//Autocomplete
bool isAutoCompleteCycling = false;
int autoCompleteIndex = 0;
int autoCompleteTextSize = 0;
float autoCompleteGreen[4] = { 0.0235f, 0.925f, 0.0196f, 1.0f };
std::string autoCompleteSubstring = "";
bool didGreenForThisText = false;

bool isShift = false;
bool isAlt = false;
bool isCtrl = false;

int superScrollAmount = 6;


bool checkForIgnoreKeys(int key) {
	switch (key) {
	case 187:
	case 200:
	case 201:
	case 202:
	case 203:
	case 204:
	case 205:
	case 161:
	case 162:
	case 153:
	case 167:
	case 168:
	case 169:
	case 170:
	case 171:
	case 172:
	case 173:
	case 174:
	case 175:
	case 176:
	case 177:
	case 194:
	case 198: //num lock
	case 27:
	case 10://newline
	case 151:
		return true;
	default:
		return false;
	}
}

//is this the same for other keyboard layouts?
int getSymbolKey(int key) {
	switch (key) {
	case 48:
		return ')';
	case 49:
		return '!';
	case 50:
		return '@';
	case 51:
		return '#';
	case 52:
		return '$';
	case 53:
		return '%';
	case 54:
		return '^';
	case 55:
		return '&';
	case 56:
		return '*';
	case 57:
		return '(';
	default:
		return '!';
	}
}

void InternalConsole::clearConsole() {
	cursorPos = 0;
	textBuffer.clear();
}

void InternalConsole::closeConsole() {
	consoleOpen = false;
	fullConsole = false;
	//v1.2.0-alpha: shift would stick when con closed while holding
	isShift = false;
	isAlt = false;
	isCtrl = false;

	autoCompleteSubstring = "";
	didGreenForThisText = false;
	autoCompleteIndex = 0;
	autoCompleteTextSize = 0;
	isAutoCompleteCycling = false;
	int lastSearchLength = -1;
	int resultCount = 0;
	searchResults.clear();
	InternalConsole::clearConsole();
	GameUtil::blockGameInput(false);
	//GameUtil::showMouseCursor(false);
	//GameUtil::enableMouseRecenter(true);
	//GameUtil::enableMouseGameEvents(true);
}

void convertSubstringToGreen(std::string& textBuffer, const std::string& autoCompleteSubstring) {
	if (didGreenForThisText) {
		return;
	}
	//v1.2.8-alpha: fix green substr for / search. Removes /
	std::string noslash = textBuffer;
	if (!noslash.empty() && noslash[0] == '/') {
		noslash.erase(0, 1);
	}
	std::string noslash2 = autoCompleteSubstring;
	if (!noslash2.empty() && noslash2[0] == '/') {
		noslash2.erase(0, 1);
	}
	std::string lowerBuffer = GameUtil::toLower(noslash);
	std::string lowerSubstring = GameUtil::toLower(noslash2);

	size_t pos = lowerBuffer.find(lowerSubstring);
	if (pos != std::string::npos) {
		textBuffer.insert(pos, "^2");
		pos += 2 + noslash2.length();
		textBuffer.insert(pos, "^7");
	}
}

int scrollbarBaseX;
int scrollbarTrackHeight;
int sliderHeight;
int sliderOffsetY;
void drawFullConsoleOutput(int windowWidth, int windowHeight) {
	int outputBaseX = (margin - borderSize) + 5;
	int outputBaseY = (margin - borderSize) + consoleHeight + fullConsoleGap + 16;
	int outputMaxY = windowHeight - margin - 14;

	int scrollbarBorderSize = 2;
	maxLines = (outputMaxY - outputBaseY) / lineSpacing;

	//scrollbar
	int scrollbarWidth = 10;
	scrollbarBaseX = windowWidth - outputBaseX - scrollbarWidth;
	int scrollbarBaseY = outputBaseY;
	int scrollbarMaxY = outputMaxY;
	scrollbarTrackHeight = scrollbarMaxY - outputBaseY;
	sliderOffsetY = 0;

	//Calculating slider height
	if (outputStack.size() > 0) {
		sliderHeight = (scrollbarTrackHeight * maxLines) / outputStack.size();
		if (sliderHeight < 15) {
			sliderHeight = 15;
		}
	}

	//Calculating slider position
	if (outputStack.size() - maxLines > 0) {
		//position based on top left
		sliderOffsetY = ((scrollbarTrackHeight - sliderHeight) * outputStackSeekPos) / (outputStack.size() - maxLines);
	}

	//if not enough content to scroll, reset
	if (maxLines > outputStack.size()) {
		sliderHeight = scrollbarTrackHeight;
		sliderOffsetY = 0;
	}


	//track bg
	Functions::_R_AddCmdDrawStretchPic(scrollbarBaseX, scrollbarBaseY, scrollbarWidth, scrollbarTrackHeight, 0.0f, 0.0f, 0.0f, 0.0f, scrollbarTrackColorPrimary, mtl_white);
	//track fg
	Functions::_R_AddCmdDrawStretchPic(scrollbarBaseX + scrollbarBorderSize, scrollbarBaseY + scrollbarBorderSize, scrollbarWidth - (scrollbarBorderSize * 2), scrollbarTrackHeight - (scrollbarBorderSize * 2), 0.0f, 0.0f, 0.0f, 0.0f, scrollbarTrackColorSecondary, mtl_white);

	//slider bg
	Functions::_R_AddCmdDrawStretchPic(scrollbarBaseX, scrollbarBaseY + sliderOffsetY, scrollbarWidth, sliderHeight, 0.0f, 0.0f, 0.0f, 0.0f, scrollbarSliderColorSecondary, mtl_white);
	//slider fg
	Functions::_R_AddCmdDrawStretchPic(scrollbarBaseX + scrollbarBorderSize, scrollbarBaseY + scrollbarBorderSize + sliderOffsetY, scrollbarWidth - (scrollbarBorderSize * 2), sliderHeight - (scrollbarBorderSize * 2), 0.0f, 0.0f, 0.0f, 0.0f, scrollbarSliderColorPrimary, mtl_white);

	for (int i = 0; outputBaseY + (i * lineSpacing) < outputMaxY; i++) {
		if ((outputStackSeekPos + i) >= 0 && (outputStackSeekPos + i) < outputStack.size()) {
			if (outputLogLevel.at(outputStackSeekPos + i) == 0) {
				Functions::_R_AddCmdDrawText(outputStack.at(outputStackSeekPos + i).c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, outputBaseX, outputBaseY + (i * lineSpacing), 1.0f, 1.0f, 0.0f, colorWhite, 0.0f);
				continue;
			}
			if (outputLogLevel.at(outputStackSeekPos + i) == 1) {
				Functions::_R_AddCmdDrawText(outputStack.at(outputStackSeekPos + i).c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, outputBaseX, outputBaseY + (i * lineSpacing), 1.0f, 1.0f, 0.0f, warningColor, 0.0f);
				continue;
			}
			if (outputLogLevel.at(outputStackSeekPos + i) == 2) {
				Functions::_R_AddCmdDrawText(outputStack.at(outputStackSeekPos + i).c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, outputBaseX, outputBaseY + (i * lineSpacing), 1.0f, 1.0f, 0.0f, errorColor, 0.0f);
				continue;
			}
			if (outputLogLevel.at(outputStackSeekPos + i) == 3) {
				Functions::_R_AddCmdDrawText(outputStack.at(outputStackSeekPos + i).c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, outputBaseX, outputBaseY + (i * lineSpacing), 1.0f, 1.0f, 0.0f, devColor, 0.0f);
				continue;
			}
		}
	}
}

// 0 - Info (default)
// 1 - Warning
// 2 - Error
// 3 - SPMod Developer
void InternalConsole::addToOutputStack(std::string s, int level) {
	size_t newlinePos = s.find_first_of("\n\r");
	//v1.2.8-alpha: Internal Console newline fix
	//if a newline character is found, split the string and recursively call the function
	if (newlinePos != std::string::npos) {
		//if the newline is the last character
		if (newlinePos == s.length() - 1) {
			s.erase(newlinePos, 1);
			outputStack.push_back(s);
			outputLogLevel.push_back(level);
		}
		else {
			//add the substring before the newline character to the output stack
			std::string beforeNewline = s.substr(0, newlinePos);
			outputStack.push_back(beforeNewline);
			outputLogLevel.push_back(level);

			//recursively call the function with the remaining string after the newline character
			std::string remaining = s.substr(newlinePos + 1);
			addToOutputStack(remaining, level);
		}
	}
	else {
		//no newline
		outputStack.push_back(s);
		outputLogLevel.push_back(level);
	}

	//1.2.4-alpha: stick to bottom when at bottom
	if (outputStack.size() > maxLines && outputStack.size() <= outputStackSeekPos + maxLines) {
		outputStackSeekPos++;
	}
}


void drawDescriptionBox(int windowWidth, int windowHeight, const dvar_t* dvar) {
	Functions::_R_AddCmdDrawStretchPic(margin - borderSize + searchBoxOffset, (margin - borderSize) + consoleHeight * 2 + (fullConsoleGap * 2) - 1, windowWidth - (margin * 2) + (borderSize * 2) - searchBoxOffset, (lineSpacing * 2) + (textStartOffsetGapY * 2), 0.0f, 0.0f, 0.0f, 0.0f, hintBoxColorSecondary, mtl_white);
	Functions::_R_AddCmdDrawStretchPic(margin + searchBoxOffset, margin + consoleHeight * 2 + (fullConsoleGap * 2) - 1, windowWidth - (margin * 2) - searchBoxOffset, (lineSpacing * 2) - (borderSize * 2) + (textStartOffsetGapY * 2), 0.0f, 0.0f, 0.0f, 0.0f, hintBoxColorPrimary, mtl_white);
	//if (dvar->description == nullptr || dvar->description[0] == '\0') {
		Functions::_R_AddCmdDrawText("N/A", 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + searchBoxOffset + 6, margin + consoleHeight * 2 + (fullConsoleGap * 2) - 1 + lineSpacing + (textStartOffsetGapY / 2) + 1, 1.0f, 1.0f, 0.0f, colorWhite, 0.0f);
	//}
	//else {
	//	Functions::_R_AddCmdDrawTextWInternal(dvar->description, 0x7FFFFFFF, InternalConsole::consoleFont, margin + searchBoxOffset + 6, margin + consoleHeight * 2 + (fullConsoleGap * 2) - 1 + lineSpacing + (textStartOffsetGapY / 2) + 1, windowWidth - (margin * 2), 1.0f, 1.0f, 0.0f, colorWhite, 1.0f, 1.0f);
	//}

	//Domain
	std::string domainText = "Domain is PLACEHOLDER"; //+ GameUtil::getDvarDomainAsString(dvar);
	Functions::_R_AddCmdDrawText(domainText.c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + searchBoxOffset + 6, margin + consoleHeight * 2 + (fullConsoleGap * 2) + lineSpacing * 2 + (textStartOffsetGapY / 2) + 1, 1.0f, 1.0f, 0.0f, commandColor, 0.0f);
}

void drawSearchBoxResults(int windowWidth, int windowHeight) {

	if (resultCount == 0) {
		return;
	}
	if (resultCount > MAX_SEARCH_RESULTS) {
		//%i matches (too many to show here, press shift+tilde to open full console)
		std::string noMatchStr = std::to_string(resultCount) + " matches (too many to show here)";
		Functions::_R_AddCmdDrawStretchPic(margin - borderSize + searchBoxOffset, (margin - borderSize) + consoleHeight + fullConsoleGap, windowWidth - (margin * 2) + (borderSize * 2) - searchBoxOffset, (lineSpacing)+(textStartOffsetGapY * 2), 0.0f, 0.0f, 0.0f, 0.0f, hintBoxColorSecondary, mtl_white);
		Functions::_R_AddCmdDrawStretchPic(margin + searchBoxOffset, margin + consoleHeight + fullConsoleGap, windowWidth - (margin * 2) - searchBoxOffset, (lineSpacing)-(borderSize * 2) + (textStartOffsetGapY * 2), 0.0f, 0.0f, 0.0f, 0.0f, hintBoxColorPrimary, mtl_white);
		Functions::_R_AddCmdDrawText(noMatchStr.c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + searchBoxOffset + 6, margin + consoleHeight + fullConsoleGap + lineSpacing + (textStartOffsetGapY / 2) + 1, 1.0f, 1.0f, 0.0f, colorWhite, 0.0f);
	}
	else {
		Functions::_R_AddCmdDrawStretchPic(margin - borderSize + searchBoxOffset, (margin - borderSize) + consoleHeight + fullConsoleGap, windowWidth - (margin * 2) + (borderSize * 2) - searchBoxOffset, (lineSpacing * searchResults.size()) + (textStartOffsetGapY * 2), 0.0f, 0.0f, 0.0f, 0.0f, hintBoxColorSecondary, mtl_white);
		Functions::_R_AddCmdDrawStretchPic(margin + searchBoxOffset, margin + consoleHeight + fullConsoleGap, windowWidth - (margin * 2) - searchBoxOffset, (lineSpacing * searchResults.size()) - (borderSize * 2) + (textStartOffsetGapY * 2), 0.0f, 0.0f, 0.0f, 0.0f, hintBoxColorPrimary, mtl_white);
		for (int i = 0; i < searchResults.size(); i++) {
			if (searchResults.at(i)->type != DVAR_TYPE_INT64) { //is a dvar
				Functions::_R_AddCmdDrawText(DvarInterface::toUserString(searchResults.at(i)->name).c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + searchBoxOffset + 6, margin + consoleHeight + fullConsoleGap + lineSpacing + (i * lineSpacing) + (textStartOffsetGapY / 2) + 1, 1.0f, 1.0f, 0.0f, colorWhite, 0.0f);
				Functions::_R_AddCmdDrawText(GameUtil::dvarValueToString(searchResults.at(i), true, false).c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + searchBoxOffset + 6 + valueDisplayOffsetX, margin + consoleHeight + fullConsoleGap + lineSpacing + (i * lineSpacing) + (textStartOffsetGapY / 2) + 1, 1.0f, 1.0f, 0.0f, colorWhite, 0.0f);
			}
			else { //is a command
				Functions::_R_AddCmdDrawText(searchResults.at(i)->name, 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + searchBoxOffset + 6, margin + consoleHeight + fullConsoleGap + lineSpacing + (i * lineSpacing) + (textStartOffsetGapY / 2) + 1, 1.0f, 1.0f, 0.0f, commandColor, 0.0f);
			}
		}
	}
	if (resultCount == 1) { //do the description draw now
		if (searchResults.at(0)->type != DVAR_TYPE_INT64) {
			drawDescriptionBox(windowWidth, windowHeight, searchResults.at(0));
		}
	}
}

void drawSearchBoxResultSingle(int windowWidth, int windowHeight, dvar_t* dvar) {
	Functions::_R_AddCmdDrawStretchPic(margin - borderSize + searchBoxOffset, (margin - borderSize) + consoleHeight + fullConsoleGap, windowWidth - (margin * 2) + (borderSize * 2) - searchBoxOffset, (lineSpacing)+(textStartOffsetGapY * 2), 0.0f, 0.0f, 0.0f, 0.0f, hintBoxColorSecondary, mtl_white);
	Functions::_R_AddCmdDrawStretchPic(margin + searchBoxOffset, margin + consoleHeight + fullConsoleGap, windowWidth - (margin * 2) - searchBoxOffset, (lineSpacing)-(borderSize * 2) + (textStartOffsetGapY * 2), 0.0f, 0.0f, 0.0f, 0.0f, hintBoxColorPrimary, mtl_white);
	Functions::_R_AddCmdDrawText(DvarInterface::toUserString(dvar->name).c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + searchBoxOffset + 6, margin + consoleHeight + fullConsoleGap + lineSpacing + (textStartOffsetGapY / 2) + 1, 1.0f, 1.0f, 0.0f, colorWhite, 0.0f);
	Functions::_R_AddCmdDrawText(GameUtil::dvarValueToString(dvar, true, false).c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + searchBoxOffset + 6 + valueDisplayOffsetX, margin + consoleHeight + fullConsoleGap + lineSpacing + (textStartOffsetGapY / 2) + 1, 1.0f, 1.0f, 0.0f, colorWhite, 0.0f);
}


void findMatchingDvars(const dvar_t* dvar) {
	//v1.2.8-alpha: allow searching with "/"
	std::string noslash = textBuffer;
	if (!noslash.empty() && noslash[0] == '/') {
		noslash.erase(0, 1);
	}

	//v1.2.8-alpha: Make only dvar in search; dont search with the value being set. Done by splitting at space
	size_t pos = textBuffer.find(' ');
	if (pos != std::string::npos) {
		noslash = textBuffer.substr(0, pos);
	}
	if (GameUtil::toLower(DvarInterface::toUserString(std::string(dvar->name))).find(GameUtil::toLower(noslash)) != std::string::npos) {
		if (searchResults.size() >= MAX_SEARCH_RESULTS == false) { //dont waste time adding if not using
			searchResults.push_back(dvar);
		}
		resultCount++;
	}
}


void calculateSearchBoxResults(int windowWidth, int windowHeight) {
	size_t spacePos = textBuffer.find(' ');
	std::string cmdNoSpace = textBuffer.substr(0, spacePos);
	if (spacePos != std::string::npos) { //draw description when space after input
		if (Functions::_Dvar_FindVar(DvarInterface::toEngineString(cmdNoSpace).c_str()) != nullptr) {
			//if (Functions::_Dvar_FindVar(cmdNoSpace.c_str())->type != DVAR_TYPE_INT64) {
				drawDescriptionBox(windowWidth, windowHeight, Functions::_Dvar_FindVar(DvarInterface::toEngineString(cmdNoSpace).c_str()));
				drawSearchBoxResultSingle(windowWidth, windowHeight, Functions::_Dvar_FindVar(DvarInterface::toEngineString(cmdNoSpace).c_str()));
			//}
		}
	}

	if (textBuffer.length() == 0) { //dont search without input
		lastSearchLength = 0;
		return;
	}
	if (lastSearchLength == textBuffer.length() || isAutoCompleteCycling) { //dont search if already searched this input or isAutoCompleteCycling
		//v1.2.8-alpha: Prevent drawing when no space since changed how it works
		if (spacePos == std::string::npos) {
			drawSearchBoxResults(windowWidth, windowHeight);
		}
		didGreenForThisText = true;
	}
	else {
		didGreenForThisText = false;
		resultCount = 0;
		searchResults.clear();

		//all dvars from dvar interface
		for (const auto& pair : DvarInterface::userToEngineMap) {
			const std::string& value = pair.first;
			findMatchingDvars(Functions::_Dvar_FindVar(DvarInterface::toEngineString(value).c_str()));
		}
	
		//TODO: parse commands from in-engine

		lastSearchLength = textBuffer.length();
		if (searchResults.size() == 0) {
			return;
		}
		//v1.2.8-alpha: Prevent drawing when space since changed how it works
		if (spacePos == std::string::npos) {
			drawSearchBoxResults(windowWidth, windowHeight);
		}
	}
}

void drawConsole() {
	int windowWidth = *(int*)(0x1DA11E8_b); //this isnt actually window size i think its bink player size
	int windowHeight = *(int*)(0x1DA11EC_b);
	if (borderSize > 1) {
		searchBoxOffset = 96 + borderSize;
	}


	//CustomCommands::CG_DrawVersion(windowWidth, windowHeight);

	if (consoleOpen) {
		//v1.1.1-prealpha bug fix: Prevent keycatcher change from other ingame events
		GameUtil::blockGameInput(true);
		//draw bg/border first
		if (!mtl_white) {
			Console::print("!mtl_white");
			return;
		}
		if (!InternalConsole::consoleFont) {
			Console::print("!InternalConsole::consoleFont");
			return;
		}
		Functions::_R_AddCmdDrawStretchPic(margin - borderSize, margin - borderSize, windowWidth - (margin * 2) + (borderSize * 2), consoleHeight + (borderSize * 2), 0.0f, 0.0f, 0.0f, 0.0f, colorSecondary, mtl_white);
		Functions::_R_AddCmdDrawStretchPic(margin, margin, windowWidth - (margin * 2), consoleHeight, 0.0f, 0.0f, 0.0f, 0.0f, colorPrimary, mtl_white);
		//draw Left Text
		Functions::_R_AddCmdDrawText("S2MP-Mod >", 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + borderSize + 4, margin + consoleHeight - 4, 1.0f, 1.0f, 0.0f, colorTextLabel, 0);

		//draw cursor and text input
		if (isAutoCompleteCycling) {
			if (textBuffer.length() >= autoCompleteTextSize) {
				convertSubstringToGreen(textBuffer, autoCompleteSubstring);
				cursorPos = textBuffer.length();
			}
		}
		Functions::_R_AddCmdDrawTextWithCursor(textBuffer.c_str(), 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + borderSize + 88, margin + consoleHeight - 3, 1.0f, 1.0f, 0.0f, colorWhite, 0.0f, cursorPos, '|');
		if (fullConsole) {
			//										(float x, float y, float w, float h, float s0, float t0, float s1, float t1, float* color, Material* material);
			//for the full console need to draw each border separately due to transparency
			// full for example
			//Functions::_R_AddCmdDrawStretchPicInternal(margin - borderSize, (margin - borderSize) + consoleHeight + fullConsoleGap, windowWidth - (margin * 2) + (borderSize * 2), (windowHeight - consoleHeight - fullConsoleGap - (margin*2)) + (borderSize * 2), 0.0f, 0.0f, 0.0f, 0.0f, colorFullSecondary, mtl_white);
			//left
			Functions::_R_AddCmdDrawStretchPic(margin - borderSize, (margin - borderSize) + consoleHeight + fullConsoleGap, borderSize, (windowHeight - consoleHeight - fullConsoleGap - (margin * 2)) + (borderSize * 2), 0.0f, 0.0f, 0.0f, 0.0f, colorFullSecondary, mtl_white);
			//right
			Functions::_R_AddCmdDrawStretchPic(windowWidth - (margin), (margin - borderSize) + consoleHeight + fullConsoleGap, borderSize, (windowHeight - consoleHeight - fullConsoleGap - (margin * 2)) + (borderSize * 2), 0.0f, 0.0f, 0.0f, 0.0f, colorFullSecondary, mtl_white);
			//top
			Functions::_R_AddCmdDrawStretchPic(margin - borderSize, (margin - borderSize) + consoleHeight + fullConsoleGap, windowWidth - (margin * 2) + (borderSize * 2), borderSize, 0.0f, 0.0f, 0.0f, 0.0f, colorFullSecondary, mtl_white);
			//bottom
			Functions::_R_AddCmdDrawStretchPic(margin - borderSize, windowHeight - margin, windowWidth - (margin * 2) + (borderSize * 2), borderSize, 0.0f, 0.0f, 0.0f, 0.0f, colorFullSecondary, mtl_white);
			//bg
			Functions::_R_AddCmdDrawStretchPic(margin, margin + consoleHeight + fullConsoleGap, windowWidth - (margin * 2) /*- (borderSize)*/, (windowHeight - consoleHeight - fullConsoleGap) - (margin * 2), 0.0f, 0.0f, 0.0f, 0.0f, colorFullPrimary, mtl_white);


			//Bottom Left Build Version
			Functions::_R_AddCmdDrawText("S2 MP build 1.25 Sun Nov 24 16:36:17 2019 win64", 0x7FFFFFFF, InternalConsole::consoleFont, 0, 0, InternalConsole::consoleFont->pixelHeight, margin + borderSize + 2, (windowHeight - margin - borderSize), 1.0f, 1.0f, 0.0f, fullConBuildVerCol, 0);
			drawFullConsoleOutput(windowWidth, windowHeight);

		}
		calculateSearchBoxResults(windowWidth, windowHeight);
	}
	else {
		DevDraw::render(windowWidth, windowHeight);
	}
}

void InternalConsole::toggleConsole() {
	consoleOpen = !consoleOpen;
	isCmdStackSeeking = false;
	//ConsoleStrings::setupDvarDescriptions();
	if (!consoleOpen) { //console closed
		closeConsole();
	}
	else { //console open
		GameUtil::blockGameInput(true);
		//GameUtil::showMouseCursor(true);
		//GameUtil::enableMouseRecenter(false);
		//GameUtil::enableMouseGameEvents(false);
	}
}

void InternalConsole::toggleFullConsole() {
	if (!consoleOpen) {
		InternalConsole::toggleConsole();
		fullConsole = true;
		return;
	}
	if (consoleOpen) {
		fullConsole = !fullConsole;
	}
}

void handleKeys(int client, int key, int down) {
	//Console::printf("Client %d hit key %d | down: %d", client, key, down);
	//if (!consoleOpen && down == 1) { //check for keybind
		//Binds::checkAndExecBind(key);
	//}
	//if (printKeystrokes) {
	//	Console::printf("Client %d hit key %d | down: %d", client, key, down);
	//}

	//Noclip::sprintBindHandler(key, down);

	if (key != 9) { //if not tab (for canceling autocomplete)
		if (isAutoCompleteCycling) {
			if (autoCompleteIndex - 1 < 0) {
				textBuffer = DvarInterface::toUserString(searchResults.at(searchResults.size())->name);
			}
			else {
				textBuffer = DvarInterface::toUserString(searchResults.at(autoCompleteIndex - 1)->name);
			}

			cursorPos = textBuffer.length();
		}
		isAutoCompleteCycling = false;
		autoCompleteIndex = 0;

	}
	//Shift key
	if (key == 160 && down == 1) {
		isShift = true;
		return;
	}
	if (key == 160 && down == 0) {
		isShift = false;
		return;
	}

	//Alt Key
	if (key == 158 && down == 1) {
		isAlt = true;
		return;
	}
	if (key == 158 && down == 0) {
		isAlt = false;
		return;
	}

	//Ctrl Key
	if (key == 159 && down == 1) {
		isCtrl = true;
		return;
	}
	if (key == 159 && down == 0) {
		isCtrl = false;
		return;
	}

	//v1.2.8-alpha: Ctrl + Home Jump to top of output window
	if (key == 165 && down == 1 && isCtrl && fullConsole) {
		outputStackSeekPos = 0;
		return;
	}
	//end
	if (key == 166 && down == 1 && isCtrl && fullConsole) {
		if (outputStack.size() < maxLines) {
			outputStackSeekPos = 0;
		}
		else {
			outputStackSeekPos = outputStack.size() - maxLines;
		}

		return;
	}

	//v1.2.8-alpha: Home & End support
	//165: Home
	if (key == 165 && down == 1 && !isCtrl) {
		cursorPos = 0;
		return;
	}
	//166: End
	if (key == 166 && down == 1) {
		cursorPos = textBuffer.length();
		return;
	}

	//Keys to ignore
	if (checkForIgnoreKeys(key)) {
		return;
	}

	if (down != 1) {
		return;
	}
	// ` or ~ or ²
	//178 is also F12 on US layout
	if (key == 96 || key == 126 || key == 178) {

		isAutoCompleteCycling = false;
		if (isShift) {
			InternalConsole::toggleFullConsole();
			return;
		}
		else {
			InternalConsole::toggleConsole();
		}
		return;
	}

	if (consoleOpen) {
		//Stack Seeking for Full Int Con
		if (key == 206 || key == 163) {
			if (!isCtrl) {
				if ((outputStackSeekPos)+maxLines < outputStack.size()) {
					outputStackSeekPos++;
				}
			}
			else { //v1.2.8-alpha: Ctrl + Scroll = super scroll
				if ((outputStackSeekPos)+maxLines + superScrollAmount < outputStack.size()) {
					outputStackSeekPos += superScrollAmount;
				}
				else {
					if (outputStack.size() < maxLines) {
						outputStackSeekPos = 0;
					}
					else {
						outputStackSeekPos = outputStack.size() - maxLines;
					}
				}
			}

			return;
		}
		if (key == 207 || key == 164) {
			if (!isCtrl) {
				if ((outputStackSeekPos) > 0) {
					outputStackSeekPos--;
				}
			}
			else { //v1.2.8-alpha: Ctrl + Scroll = super scroll
				if ((outputStackSeekPos)-superScrollAmount > 0) {
					outputStackSeekPos -= superScrollAmount;
				}
				else {
					outputStackSeekPos = 0;
				}
			}
			return;
		}

		//cmdStackSeekPos represents INDEX not SIZE
		//Stack seeking for cmd stack with arrow keys
		if (key == 154) { //UP ARROW
			if (cmdStack.size() == 0) { //cant seek through empty stack
				return;
			}
			isCmdStackSeeking = true;
			if (cmdStackSeekPos == -1) { //should only happen on first seek of session
				cmdStackSeekPos = cmdStack.size() - 1;
				textBuffer = cmdStack.at(cmdStackSeekPos);
				cursorPos = textBuffer.length();
				return;
			}
			if (cmdStackSeekPos - 1 < 0) { //prevent out of bounds
				return;
			}
			cmdStackSeekPos--;
			textBuffer = cmdStack.at(cmdStackSeekPos);
			cursorPos = textBuffer.length();
			return;
		}
		if (key == 155) { //DOWN ARROW
			if (!isCmdStackSeeking) { //dont start seek with down arrow, only up
				return;
			}
			if (cmdStackSeekPos + 1 >= cmdStack.size()) { //prevent out of bounds
				return;
			}

			cmdStackSeekPos++;
			textBuffer = cmdStack.at(cmdStackSeekPos);
			cursorPos = textBuffer.length();
			return;
		}

		if (isCtrl && key == 118) { //CTRL + V: PASTE

			std::string paste = GameUtil::getStringFromClipboard();
			textBuffer.insert(cursorPos, paste);
			cursorPos += paste.length();
			return;
		}
		//tab
		if (key == 9) {
			if (resultCount > MAX_SEARCH_RESULTS || textBuffer.length() == 0) {
				return;
			}
			if (searchResults.size() == 0) {
				return;
			}
			//v1.2.8-alpha: hinder wants this. when dvar locked in (space in string), tab will print all values of it
			//if (textBuffer.find(' ') != std::string::npos) {
			//	doHinderDvarValPrint();
			//	return;
			//}
			if (!isAutoCompleteCycling) {
				isAutoCompleteCycling = true;
				autoCompleteSubstring = textBuffer;
				autoCompleteTextSize = textBuffer.length();
			}
			if (autoCompleteIndex >= searchResults.size()) {
				autoCompleteIndex = 0;
			}

			didGreenForThisText = false;
			textBuffer = DvarInterface::toUserString(searchResults.at(autoCompleteIndex)->name);
			cursorPos = textBuffer.length();
			autoCompleteIndex++;
			return;
		}

		if (key == 13 || key == 192) {//enter
			isCmdStackSeeking = false;
			cmdStackSeekPos = -1;
			//v1.2.8-alpha: empty textbuffer causes crash (did not used to happen)
			if (textBuffer.length() == 0) {
				return;
			}
			cmdStack.push_back(textBuffer);
			//v1.4.0-Alpha whitespace only crash fix
			if (!GameUtil::isOnlyWhitespace(textBuffer)) {
				Console::execCmd(textBuffer);
			}
			
			InternalConsole::clearConsole();
			return;
		}
		if (key == 127) { //backspace
			if (textBuffer.length() != 0 && cursorPos != 0) {
				textBuffer.erase(cursorPos - 1, 1);
				cursorPos -= 1;
			}
			return;
		}
		if (key == 157) {  //arrow right
			if (cursorPos < textBuffer.length()) {
				cursorPos += 1;
			}
			return;
		}
		if (key == 156) {  //arrow right
			if (cursorPos > 0) {
				cursorPos -= 1;
			}
			return;
		}

		//handle shift uppercase
		if (isShift) {
			if (key >= 'a' && key <= 'z') {
				key -= 32;
			}
			if (key >= '0' && key <= '9') {
				key = getSymbolKey(key);
			}
			if (key == '\'') {
				key = '"';
			}
			if (key == '-') {
				key = '_';
			}
			if (key == '=') {
				key = '+';
			}
			if (key == ';') {
				key = ':';
			}
		}

		//numpad -
		if (key == 196) {
			key = '-';
		}
		if (key == 195) {
			key = '/';
		}
		if (key == 199) {
			key = '*';
		}
		if (key == 197) {
			key = '+';
		}

		if (cursorPos <= textBuffer.length()) {
			textBuffer.insert(cursorPos, 1, static_cast<char>(key));
			cursorPos += 1;
		}
	}
}

void hook_CL_KeyEvent(int client, int key, int down) {
	handleKeys(client, key, down);
    _CL_KeyEvent(client, key, down);
}

void R_EndFrame_hookfunc() {
	int winWidth = *(int*)(0x1DA11E8_b); //this isnt actually window size i think its bink player size
	int winHeight = *(int*)(0x1DA11EC_b);

	drawConsole();
	_EndFrame();
}


void renderHookInit() {
    void* target = (void*)(0x92E430_b); //endframe
    MH_Initialize();

    MH_CreateHook(target, &R_EndFrame_hookfunc, reinterpret_cast<void**>(&_EndFrame));
    MH_EnableHook(target);

    //CL_KeyEvent
    MH_CreateHook(reinterpret_cast<void*>(0x4C1A20_b), &hook_CL_KeyEvent, reinterpret_cast<void**>(&_CL_KeyEvent));
    MH_EnableHook(reinterpret_cast<void*>(0x4C1A20_b));
}


void InternalConsole::init() {
    renderHookInit();
	//TODO: add a wait until these assets are loaded
	Console::printf("Registering font: %s", "fonts/consolefont");
	InternalConsole::consoleFont = Functions::_R_RegisterFont("fonts/consolefont", 15);
	Console::printf("Registering material: %s", "white");
	mtl_white = Functions::_Material_RegisterHandle("white");
}