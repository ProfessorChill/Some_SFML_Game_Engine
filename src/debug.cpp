#include "debug.hpp"

namespace dbg
{
void printMessage(const char *msg, Urgency urgency)
{
	Modifier def(Code::FG_DEFAULT);

	switch (urgency) {
		case Urgency::DEFAULT: {
			std::cout << def << "[Message] " << msg << "\n";
			break;
		}

		case Urgency::WARNING: {
			Modifier yel(Code::FG_YELLOW);
			std::cout << "[" << yel << "Warning" << def << "] " << msg << "\n";
			break;
		}

		case Urgency::ERROR: {
			Modifier red(Code::FG_RED);
			std::cout << "[" << red << "Error" << def << "] " << msg << "\n";
			break;
		}
	}
}

tinyxml2::XMLError *handleXMLError(tinyxml2::XMLError err)
{
	switch (err) {
		case tinyxml2::XMLError::XML_SUCCESS:
			return nullptr;

		case tinyxml2::XMLError::XML_NO_ATTRIBUTE:
			printMessage("XML attribute not found.", Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_WRONG_ATTRIBUTE_TYPE:
			printMessage("XML attribute is the wrong type.", Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_FILE_NOT_FOUND:
			printMessage("XML file not found!", Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_FILE_COULD_NOT_BE_OPENED:
			printMessage("XML file could not be opened, check permissions on the file.",
				     Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_FILE_READ_ERROR:
			printMessage(
			    "XML there was an error reading the file, please check the file.",
			    Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT:
			printMessage("XML there was an error parsing the element.", Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_PARSING_ATTRIBUTE:
			printMessage("XML there was an error parsing the attribute.",
				     Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_PARSING_TEXT:
			printMessage("XML there was an error parsing text.", Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_PARSING_CDATA:
			printMessage("XML there was an error parsing CData.", Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_PARSING_COMMENT:
			printMessage("XML there was an error parsing a comment.", Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_PARSING_DECLARATION:
			printMessage("XML there was an error parsing a declaration.",
				     Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_PARSING_UNKNOWN:
			printMessage("XML there was an unknown error while parsing.",
				     Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_EMPTY_DOCUMENT:
			printMessage("XML the document provided was empty and cannot be parsed.",
				     Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_MISMATCHED_ELEMENT:
			printMessage("XML there was an error with mismatched elements, please "
				     "check opening and closing tags.",
				     Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ERROR_PARSING:
			printMessage("XML there was an error parsing.", Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_CAN_NOT_CONVERT_TEXT:
			printMessage("XML there was an error converting text.", Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_NO_TEXT_NODE:
			printMessage("XML there was no text node found.", Urgency::ERROR);
			break;

		case tinyxml2::XMLError::XML_ELEMENT_DEPTH_EXCEEDED:
			printMessage("XML exceeded the maximum element depth!", Urgency::ERROR);
			break;

		default:
			printMessage("XML a unsupported error occured.", Urgency::ERROR);
			break;
	}

	return new tinyxml2::XMLError(err);
}
}; // namespace dbg
