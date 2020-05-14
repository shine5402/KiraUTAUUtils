#include "otoentry.h"
#include <utility>
#include <QHash>
#include <QDebug>

OtoEntry::OtoEntry(QString fileName,
                   QString alias,
                   double left,
                   double consonant,
                   double right,
                   double preUtterance,
                   double overlap,
                   QObject *parent)
    : QObject(parent), m_fileName(std::move(fileName)), m_alias(std::move(alias)),
      m_left(left), m_consonant(consonant),m_right(right),
      m_preUtterance(preUtterance),m_overlap(overlap),m_valid(true)
{

}

OtoEntry::OtoEntry(const QString& otoString)
{
    if (otoString.isEmpty())
    {
        setError(EmptyOtoString);
        return;
    }
    auto fileNameEndIndex = otoString.indexOf("=");
    if (fileNameEndIndex != -1)
    {
        setFileName(otoString.left(fileNameEndIndex));
        if (fileName().isEmpty())
        {
            setError(EmptyFileName);
            return;
        }
        auto parameterString = otoString.mid(fileNameEndIndex + 1);
        setAlias(parameterString.section(",",0,0));

        constexpr int parameterCount = 5;
        std::array<bool,parameterCount> doubleConvertOks{};
        enum parameterID{LEFT, CONSONANT, RIGHT, PREUTTERANCE, OVERLAP};

        setLeft(parameterString.section(",",1,1).toDouble(&doubleConvertOks[parameterID::LEFT]));
        setConsonant(parameterString.section(",",2,2).toDouble(&doubleConvertOks[parameterID::CONSONANT]));
        setRight(parameterString.section(",",3,3).toDouble(&doubleConvertOks[parameterID::RIGHT]));
        setPreUtterance(parameterString.section(",",4,4).toDouble(&doubleConvertOks[parameterID::PREUTTERANCE]));
        setOverlap(parameterString.section(",",5,5).toDouble(&doubleConvertOks[parameterID::OVERLAP]));

        for (int i = parameterID::LEFT; i <= parameterID::OVERLAP; i++)
        {
            if (!doubleConvertOks.at(i))
            {
                switch (i) {
                case LEFT: setError(LeftConvertFailed);break;
                case CONSONANT: setError(ConsonantConvertFailed);break;
                case RIGHT: setError(RightConvertFailed);break;
                case PREUTTERANCE: setError(PreUtteranceConvertFailed);break;
                case OVERLAP: setError(OverlapConvertFailed);break;
                }
                return;
            }
        }
    }
    else{
        setError(FileNameSeparatorNotFound);
        return;
    }
    setValid(true);
}

OtoEntry::OtoEntry(const OtoEntry& other) : QObject(other.parent())
{
    m_fileName = other.m_fileName;
    m_alias = other.m_alias;
    m_left = other.m_left;
    m_consonant = other.m_consonant;
    m_right = other.m_right;
    m_preUtterance = other.m_preUtterance;
    m_overlap = other.m_overlap;

    m_error = other.m_error;
    m_valid = other.m_valid;
}

OtoEntry& OtoEntry::operator=(const OtoEntry& rhs)
{
    if (*this != rhs)
    {
        m_fileName = rhs.m_fileName;
        m_alias = rhs.m_alias;
        m_left = rhs.m_left;
        m_consonant = rhs.m_consonant;
        m_right = rhs.m_right;
        m_preUtterance = rhs.m_preUtterance;
        m_overlap = rhs.m_overlap;

        m_error = rhs.m_error;
        m_valid = rhs.m_valid;
    }
    return *this;
}

QString OtoEntry::toString() const
{
    return QString("%1=%2,%3,%4,%5,%6,%7")
            .arg(m_fileName, m_alias)
            .arg(m_left,0,'f',3)
            .arg(m_consonant,0,'f',3)
            .arg(m_right,0,'f',3)
            .arg(m_preUtterance,0,'f',3)
            .arg(m_overlap,0,'f',3);
}

bool OtoEntry::operator==(const OtoEntry& rhs) const
{
    return fileName() == rhs.fileName() && alias() == rhs.alias()
            && left() == rhs.left() && consonant() == rhs.consonant()
            && right() == rhs.right() && preUtterance() == rhs.preUtterance()
            && overlap() == rhs.overlap();
}

bool OtoEntry::operator!=(const OtoEntry& rhs) const
{
    return !(*this == rhs);
}

void OtoEntry::setError(OtoEntry::OtoEntryError error)
{
    m_error = error;
    setValid(false);
}

void OtoEntry::setValid(bool valid)
{
    m_valid = valid;
}

QString OtoEntry::fileName() const
{
    return m_fileName;
}

QString OtoEntry::alias() const
{
    return m_alias;
}

double OtoEntry::left() const
{
    return m_left;
}

void OtoEntry::setLeft(double value)
{
    m_left = value;
    emit leftChanged();
}

double OtoEntry::consonant() const
{
    return m_consonant;
}

void OtoEntry::setConsonant(double value)
{
    m_consonant = value;
    emit consonantChanged();
}

double OtoEntry::right() const
{
    return m_right;
}

void OtoEntry::setRight(double value)
{
    m_right = value;
    emit rightChanged();
}

double OtoEntry::preUtterance() const
{
    return m_preUtterance;
}

void OtoEntry::setPreUtterance(double value)
{
    m_preUtterance = value;
    emit preUtteranceChanged();
}

double OtoEntry::overlap() const
{
    return m_overlap;
}

void OtoEntry::setOverlap(double value)
{
    m_overlap = value;
    emit overlapChanged();
}

OtoEntry::OtoEntryError OtoEntry::error() const
{
    return m_error;
}

QString OtoEntry::errorString() const
{
    static const QHash<OtoEntryError,QString> errorStrings{
        {UnknownError, "Unknown Error"},
        {FileNameSeparatorNotFound, "The separator between fileName and alias are not found."},
        {LeftConvertFailed, "Convert left string to double failed."},
        {ConsonantConvertFailed, "Convert consonant string to double failed."},
        {RightConvertFailed, "Convert right string to double failed."},
        {PreUtteranceConvertFailed, "Convert preUtterance string to double failed."},
        {OverlapConvertFailed, "Convert overlap string to double failed."},
        {EmptyOtoString, "The provided string is empty"},
        {EmptyFileName, "The fileName is empty"},
    };

    return errorStrings.value(error());
}

void OtoEntry::setAlias(const QString& value)
{
    m_alias = value;
    emit aliasChanged();
}

void OtoEntry::setFileName(const QString& value)
{
    m_fileName = value;
    emit fileNameChanged();
}




QString OtoEntryFunctions::removePitchSuffix(QString alias, const QString& bottomPitch, const QString& topPitch, Qt::CaseSensitivity cs, CharacterCase pitchRangeCharacterCase, QString* pitchRemoved)
{
    auto pitchRange = getPitchStringRange(bottomPitch, topPitch, pitchRangeCharacterCase);
    for (const auto& pitch : pitchRange)
    {
        auto pitchPos = alias.lastIndexOf(pitch, -1 ,cs);
        if (pitchPos != -1)
        {
            if (pitchRemoved)
            {
                *pitchRemoved = pitch;
            }
            return removeSuffix(alias, pitch, cs);
        }
    }
    return alias;
}

QStringList OtoEntryFunctions::getPitchStringRange(const QString& bottomPitch, const QString& topPitch, CharacterCase cs){
    //BUG: 没有考虑到#
    const QString PitchNameOrder = [&]() -> QString{
            switch (cs) {
            case Upper: return "CDEFGAB";
            case Lower: return "cdefgab";
}
            return "CDEFGAB";
}();

    if (bottomPitch.isEmpty() || topPitch.isEmpty())
        return {};

    auto bottomPitchName = PitchNameOrder.indexOf(bottomPitch.at(0), 0, Qt::CaseInsensitive);
    auto topPitchName = PitchNameOrder.indexOf(topPitch.at(0), 0, Qt::CaseInsensitive);

    if (bottomPitchName == -1 || topPitchName == -1)
        return {};

    bool bottomOctaveConvertOk, topOctaveConvertOk;
    auto bottomOctave = bottomPitch.midRef(1).toInt(&bottomOctaveConvertOk);
    auto topOctave = topPitch.midRef(1).toInt(&topOctaveConvertOk);

    if (!(bottomOctaveConvertOk && topOctaveConvertOk))
        return {};

    QStringList pitchStringRange;
    for (auto currentOctave = bottomOctave; currentOctave <= topOctave; ++currentOctave)
    {
        int startPitchName = 0, endPitchName = PitchNameOrder.count() - 1;
        if (currentOctave == bottomOctave)
            startPitchName = bottomPitchName;
        if (currentOctave == topOctave)
            endPitchName = topPitchName;
        for (auto currentPitchName = startPitchName; currentPitchName <= endPitchName; ++currentPitchName)
        {
            pitchStringRange.append(QString("%1%2").arg(PitchNameOrder.at(currentPitchName)).arg(currentOctave));
        }
    }
    return pitchStringRange;
}

QString OtoEntryFunctions::removeSuffix(QString string, const QString& suffix, Qt::CaseSensitivity cs)
{
    auto suffixPos = string.lastIndexOf(suffix, -1, cs);
    if (suffixPos != -1)
    {
        return string.remove(suffixPos, suffix.count());
    }
    return string;
}

QString OtoEntryFunctions::getDigitSuffix(const QString& string, int* position)
{
    QString result{};
    for (int current = string.count() - 1; current >= 0; --current)
    {
        if (auto c = string.at(current);c.isDigit()){
            result.prepend(c);
        }
        else
        {
            if (position)
            {
                *position = current != string.count() - 1 ? current + 1 : -1;
            }
            break;
        }
    }
    return result;
}