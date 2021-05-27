#include "toolbox_string.h"

#include <QString>
#include <QRegularExpression>

bool stringValidAs_azAZ09_dotSpace(const QString& text) { //@LP includes '_'
    QRegularExpression qregexp_azAZ09_dotSpace(R"(^[a-zA-Z0-9_.\s]*$)");
    QRegularExpressionMatch match = qregexp_azAZ09_dotSpace.match(text);
    return(match.hasMatch());
}
