/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/smeighan/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/smeighan/xLights/blob/master/License.txt
 **************************************************************/

#include "pixels.h"

// ***********************************************************************************************
// *                                                                                             *
// * The following table may not be accurate but it is my best understanding of protocol         *
// * compatibility. This does not mean that controllers wont show them as seperate or even that  *
// * some controllers having timing that means that things which could be equivalent with the    *
// * righting timing choice are not in reality. This makes things messy but I do think having    *
// * this documented somewhere is essential.                                                     *
// *                                                                                             *
// * I have arbitrarily chosen one of the pixel types as the group name. Typically because this  *
// * is the most common or just because that is the one I randomly chose ... live with it        *
// *                                                                                             *
// * If you make changes please add a note explaining why as I am sure there is going to be      *
// * disagreement.                                                                               *
// *                                                                                             *
// ***********************************************************************************************
//
// ----------------------------------------------------------------------------------------------------------------------------------------
// | Group Name     | Characteristics        | Equivalent                                        | Controller Support                     |
// |======================================================================================================================================|
// | APA102         | RGB, 8bit, 4 wire      | APA101, APA102, APA102C, HD107S, SK9822           |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | APA109         | RGBW, 8bit, 3 wire     | APA109, SK6812RGBW, SK6818, SM16704, UCS2904      |                                        |
// |                |                        | WS2814                                            |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | DM412          | ?, ? bit, ? wire       |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | GECE           | RGB, 5 bit, 3 wire     |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | GW6205         | ?, 12 bit, ? wire      |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | LPD6803        | RGB, 5bit, 4 wire      | D705, LPD1101, LPD6803, UCS6909, UCS6912S         |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | LPD8806        | RGB, 7 bit, 4 wire     | LPD8803, LPD8806, LPD8809, UCS8812                |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | MBI6020        | ?, 10 bit, 4 wire      |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | MY9221         | RGB, 16 bit, 4 wire    |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | MY9291         | RGBW, 16 bit, 4 wire   |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | P9813          | RGB, 6 bit, 4 wire     |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | SJ1221         | RGB, 12 bit, 3 wire    |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | SM16716        | RGB, 8 bit, 4 wire     | SM16716, SM16726                                  |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | TLS3001        | RGB, 12 bit, 3 wire    | CY3005, TLS3001, TLS3002, QED3001                 |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | TLS3008        | RGB, 8 bit, 3 wire     |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | TM1814         | RGBW, 8 bit, 3 wire    |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | TM1829         | RGB, 5 bit, 3 wire     |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | UCS8903        | RGB, 16 bit, 3 wire    |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | UCS8904        | RGBW, 16 bit, 3 wire   |                                                   |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | WS2801         | RGB, 8 bit, 4 wire     | WS2801, WS2803                                    |                                        |
// |--------------------------------------------------------------------------------------------------------------------------------------|
// | WS2811         | RGB, 8 bit, 3 wire     | APA104, APA106, CS8812, GS8202, GS8206,           |                                        |
// |                |                        | GS8208, INK1002, INK1003, LPD1882,                |                                        |
// |                |                        | LX1203, P9883, SK6812, SK6813, SK6822, SM16703,   |                                        |
// |                |                        | SM16711, SM16712, TM1803, TM1804, TM1809,         |                                        |
// |                |                        | TM1812, TM1914, UCS1903, UCS1904, UCS1909,        |                                        |
// |                |                        | UCS1912, UCS2903, UCS2909, UCS2912, UCS9812,      |                                        |
// |                |                        | WS2811, WS2812, WS2813, WS2815, WS2818            |                                        |
// ----------------------------------------------------------------------------------------------------------------------------------------
// 
// I am undecided whether making this generally available to the user helps or hinders.
// The problem is if certain pixels are not equivalent on their controller and we said they were  then we have issues
//

// This table includes some artifical types such as tm18xx which dont exist but are themselves a group within a group of compatible pixels
// These are required because some controllers list them as a type

// the "preferred" pixel type should be at the start of each list
const static std::vector<std::vector<std::string>> __equivalentPixels =
{
	{ "" }, // No protocol
	{ "ws2811", // preferred & default
	  "apa104", "apa106", "cs8812", "gs8202", "gs8206", "gs8208", "ink1002", "ink1003", "lpd1882",
	  "lx1203", "p9883", "sk6812", "sk6813", "sk6822", "sm16703", "sm16711", "sm16712", "tm1803", "tm1804", "tm1809",
	  "tm1812", "tm1914", "ucs1903", "ucs1904", "ucs1909", "ucs1912", "ucs2903", "ucs2909", "ucs2912", "ucs9812",
	  "ws2812", "ws2812b", "ws2813", "ws2815", "ws2818"
	  // artifical
	  "tm18xx", "ws281x", " ws2811v1.4", " ws2811 slow", "gs820x"
	},
	{ "apa101", "apa102", "apa102c", "hd107s", "sk9822" },
	{ "apa109", "sk6812rgbw", "sk6818", "sm16704", "ucs2904", "ws2814" },
	{ "dm412" },
	{ "gece" },
	{ "gw6205" },
	{ "d705", "lpd1101", "lpd6803", "ucs6909", "ucs6912s" },
	{ "lpd8803", "lpd8806", "lpd8809", "ucs8812" },
	{ "mib6020" },
	{ "my9221" },
	{ "my9291" },
	{ "p9813" },
	{ "sj1221" },
	{ "sm16716", "sm16726" },
	{ "cy3005", "tls3001", "tls3002", "qed3001" },
	{ "tls3008" },
	{ "tm1814", "tm1814a" },
	{ "tm1829" },
	{ "ucs8903", "ucs8903 (16)" },
	{ "ucs8904", "ucs8904 (16)" },
	{ "my9231" }, // I have not researched these ... they may fit into an existing category
	{ "dmx512" }, // I have not researched these ... they may fit into an existing category
	{ "ws2801", "ws2803" }
};

const static std::vector<std::vector<std::string>> __equivalentSerial = {
	{ "dmx" },
	{ "dmx-open", "opendmx" },
	{ "dmx-pro" },
	{ "lor" },
	{ "renard" },
	{ "genericserial" },
	{ "pixelnet" },
	{ "pixelnet-lynx" },
	{ "pixelnet-open" }
};

const static std::vector<std::string> __artificalTypes =
{
	"tm18xx", "ws281x", " ws2811v1.4", " ws2811 slow", "ucs8903 (16)", "ucs8904 (16)", "gs820x"
};

bool IsArtificialPixelType(const std::string& p)
{
	for (const auto& it : __artificalTypes) 		{
		if (p == it) return true;
	}
	return false;
}

std::string ChooseBestControllerPixel(const std::vector<std::string>& controllerPixels, const std::string& selectedPixel)
{
	// first look for an exact match in controller pixels ... if that is there return that
	if (std::find(begin(controllerPixels), end(controllerPixels), selectedPixel) != end(controllerPixels)) {
		return selectedPixel;
	}

	// if not look for the pool this type is in and look for an exact match in that group
	for (const auto& it : __equivalentPixels) {
		if (std::find(begin(it), end(it), selectedPixel) != end(it)) {
			// we found our pool
			for (const auto& it2 : it) {
				if (std::find(begin(controllerPixels), end(controllerPixels), it2) != end(controllerPixels)) {
					return it2;
				}
			}
		}
	}

	// no good match
	return "";
}

std::vector<std::string> GetAllPixelTypes(const std::vector<std::string>& controllerPixels, bool includeSerial, bool includeArtificial)
{
	auto superset = GetAllPixelTypes(includeSerial, includeArtificial);

	std::vector<std::string> res;

	for (const auto& it : superset) 		{
		if (std::find(begin(controllerPixels), end(controllerPixels), it) != end(controllerPixels)) 			{
			res.push_back(it);
		}
	}

	return res;
}

std::vector<std::string> GetAllPixelTypes(bool includeSerial, bool includeArtificial)
{
	std::vector<std::string> res;

	for (const auto& it : __equivalentPixels) 		{
		for (const auto& it2 : it) 			{
			if (includeArtificial || !IsArtificialPixelType(it2)) 				{
				res.push_back(it2);
			}
		}
	}

	if (includeSerial) 		{
		for (const auto& it : __equivalentSerial) {
			for (const auto& it2 : it) {
				res.push_back(it2);
			}
		}
	}

	return res;
}

std::vector<std::string> GetAllSerialTypes()
{
	std::vector<std::string> res;

	for (const auto& it : __equivalentSerial) {
		for (const auto& it2 : it) {
			res.push_back(it2);
		}
	}
	
	return res;
}

std::vector<std::string> GetAllSerialTypes(const std::vector<std::string>& controllerSerial)
{
	auto superset = GetAllSerialTypes();

	std::vector<std::string> res;

	for (const auto& it : superset) {
		if (std::find(begin(controllerSerial), end(controllerSerial), it) != end(controllerSerial)) {
			res.push_back(it);
		}
	}

	return res;
}

std::string ChooseBestControllerSerial(const std::vector<std::string>& controllerSerial, const std::string& selectedSerial)
{
	// first look for an exact match in controller pixels ... if that is there return that
	if (std::find(begin(controllerSerial), end(controllerSerial), selectedSerial) != end(controllerSerial)) {
		return selectedSerial;
	}

	// if not look for the pool this type is in and look for an exact match in that group
	for (const auto& it : __equivalentSerial) {
		if (std::find(begin(it), end(it), selectedSerial) != end(it)) {
			// we found our pool
			for (const auto& it2 : it) {
				if (std::find(begin(controllerSerial), end(controllerSerial), it2) != end(controllerSerial)) {
					return it2;
				}
			}
		}
	}

	// no good match
	return "";
}

bool IsPixelProtocol(const std::string& p)
{
	auto pt = GetAllPixelTypes(false, true);
	return std::find(begin(pt), end(pt), p) != end(pt);
}

bool IsSerialProtocol(const std::string& p)
{
	auto st = GetAllSerialTypes();
	return std::find(begin(st), end(st), p) != end(st);
}
