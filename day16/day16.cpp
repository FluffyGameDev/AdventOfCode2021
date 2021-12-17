#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <fmt/core.h>

using u8 = std::uint8_t;
using u64 = std::uint64_t;

enum class PacketTypeID
{
    Sum,
    Product,
    Min,
    Max,
    Litteral,
    Greater,
    Less,
    Equal
};

struct LitteralPacket;
struct SumOperatorPacket;
struct ProductOperatorPacket;
struct MinOperatorPacket;
struct MaxOperatorPacket;
struct GreaterOperatorPacket;
struct LessOperatorPacket;
struct EqualOperatorPacket;

class PacketVisitor
{
public:
    virtual u64 VisitLitteral(const LitteralPacket& litteralPacket) = 0;
    virtual u64 VisitSumOperator(const SumOperatorPacket& sumOperator) = 0;
    virtual u64 VisitProductOperator(const ProductOperatorPacket& productOperator) = 0;
    virtual u64 VisitMinOperator(const MinOperatorPacket& minOperator) = 0;
    virtual u64 VisitMaxOperator(const MaxOperatorPacket& maxOperator) = 0;
    virtual u64 VisitGreaterOperator(const GreaterOperatorPacket& greaterOperator) = 0;
    virtual u64 VisitLessOperator(const LessOperatorPacket& lessOperator) = 0;
    virtual u64 VisitEqualOperator(const EqualOperatorPacket& EqualOperator) = 0;
};

struct Packet
{
    Packet(u64 version)
        : Version{ version }
    {
    }

    u64 Version{};

    virtual u64 Accept(PacketVisitor& visitor) const = 0;
};

struct LitteralPacket : public Packet
{
    LitteralPacket(u64 version, u64 value)
        : Packet{ version }
        , Value{ value }
    {
    }

    u64 Accept(PacketVisitor& visitor) const override
    {
        return visitor.VisitLitteral(*this);
    }

    u64 Value;
};

struct OperatorPacket : public Packet
{
    OperatorPacket(u64 version)
        : Packet{ version }
    {
    }

    std::vector<std::unique_ptr<Packet>> SubPackets;
};

struct SumOperatorPacket : public OperatorPacket
{
    SumOperatorPacket(u64 version)
        : OperatorPacket{ version }
    {
    }

    u64 Accept(PacketVisitor& visitor) const override
    {
        return visitor.VisitSumOperator(*this);
    }
};

struct ProductOperatorPacket : public OperatorPacket
{
    ProductOperatorPacket(u64 version)
        : OperatorPacket{ version }
    {
    }

    u64 Accept(PacketVisitor& visitor) const override
    {
        return visitor.VisitProductOperator(*this);
    }
};

struct MinOperatorPacket : public OperatorPacket
{
    MinOperatorPacket(u64 version)
        : OperatorPacket{ version }
    {
    }

    u64 Accept(PacketVisitor& visitor) const override
    {
        return visitor.VisitMinOperator(*this);
    }
};

struct MaxOperatorPacket : public OperatorPacket
{
    MaxOperatorPacket(u64 version)
        : OperatorPacket{ version }
    {
    }

    u64 Accept(PacketVisitor& visitor) const override
    {
        return visitor.VisitMaxOperator(*this);
    }
};

struct GreaterOperatorPacket : public OperatorPacket
{
    GreaterOperatorPacket(u64 version)
        : OperatorPacket{ version }
    {
    }

    u64 Accept(PacketVisitor& visitor) const override
    {
        return visitor.VisitGreaterOperator(*this);
    }
};

struct LessOperatorPacket : public OperatorPacket
{
    LessOperatorPacket(u64 version)
        : OperatorPacket{ version }
    {
    }

    u64 Accept(PacketVisitor& visitor) const override
    {
        return visitor.VisitLessOperator(*this);
    }
};

struct EqualOperatorPacket : public OperatorPacket
{
    EqualOperatorPacket(u64 version)
        : OperatorPacket{ version }
    {
    }

    u64 Accept(PacketVisitor& visitor) const override
    {
        return visitor.VisitEqualOperator(*this);
    }
};

class BufferReader
{
public:
    BufferReader(const std::string& rawBuffer)
        : m_CurrentBit{ 0 }
    {
        size_t index{};
        u8 currentData{};
        for (index = 0; index < rawBuffer.size(); ++index)
        {

            u8 currentChar = rawBuffer[index];
            if (std::isdigit(currentChar)) { currentChar -= '0'; }
            else if (std::isalnum(currentChar)) { currentChar = currentChar - 'A' + 10; }

            currentData = currentData << 4 | currentChar;

            if (index % 2) { m_RawData.push_back(currentData); }
        }

        if (index % 2) { currentData <<= 4; m_RawData.push_back(currentData); }
    }

    u64 Read(size_t bitLength)
    {
        u64 data{};

        while (bitLength > 0)
        {
            size_t remainingBitsInByte{ 8 - m_CurrentBit % 8 };
            u8 currentByte{ m_RawData[m_CurrentBit / 8] };

            size_t readBits{ std::min(remainingBitsInByte, bitLength) };

            size_t readOffset = (remainingBitsInByte - readBits);
            u8 readMask = ((1 << readBits) - 1) << readOffset;
            u8 readData = (currentByte & readMask) >> readOffset;

            data = (data << readBits) | readData;

            bitLength -= readBits;
            m_CurrentBit += readBits;
        }

        return data;
    }

    size_t GetCurrentBitIndex() const { return m_CurrentBit; }

private:
    std::vector<u8> m_RawData;
    size_t m_CurrentBit{};
};

std::unique_ptr<Packet> ReadPacket(BufferReader& bufferReader);

std::unique_ptr<LitteralPacket> ReadLitteral(BufferReader& bufferReader, u64 packetVersion)
{
    u64 litteralValue{};
    u64 keepReading{};
    do
    {
        keepReading = bufferReader.Read(1);
        litteralValue = litteralValue << 4 | bufferReader.Read(4);
    } while (keepReading);

    return std::make_unique<LitteralPacket>(packetVersion, litteralValue);
}

void ReadOperatorSubPackets(BufferReader& bufferReader, OperatorPacket& operatorPacket)
{
    if (bufferReader.Read(1))
    {
        u64 subPacketCount{ bufferReader.Read(11) };
        for (u64 i = 0; i < subPacketCount; ++i)
        {
            operatorPacket.SubPackets.push_back(ReadPacket(bufferReader));
        }
    }
    else
    {
        size_t subPacketSectionLength{ bufferReader.Read(15) };
        size_t subPacketSectionEnd{ bufferReader.GetCurrentBitIndex() + subPacketSectionLength };
        while (bufferReader.GetCurrentBitIndex() < subPacketSectionEnd)
        {
            operatorPacket.SubPackets.push_back(ReadPacket(bufferReader));
        }
    }
}

std::unique_ptr<SumOperatorPacket> ReadSumOperator(BufferReader& bufferReader, u64 packetVersion)
{
    std::unique_ptr<SumOperatorPacket> operatorPacket{ std::make_unique<SumOperatorPacket>(packetVersion) };
    ReadOperatorSubPackets(bufferReader, *operatorPacket);
    return operatorPacket;
}

std::unique_ptr<ProductOperatorPacket> ReadProductOperator(BufferReader& bufferReader, u64 packetVersion)
{
    std::unique_ptr<ProductOperatorPacket> operatorPacket{ std::make_unique<ProductOperatorPacket>(packetVersion) };
    ReadOperatorSubPackets(bufferReader, *operatorPacket);
    return operatorPacket;
}

std::unique_ptr<MinOperatorPacket> ReadMinOperator(BufferReader& bufferReader, u64 packetVersion)
{
    std::unique_ptr<MinOperatorPacket> operatorPacket{ std::make_unique<MinOperatorPacket>(packetVersion) };
    ReadOperatorSubPackets(bufferReader, *operatorPacket);
    return operatorPacket;
}

std::unique_ptr<MaxOperatorPacket> ReadMaxOperator(BufferReader& bufferReader, u64 packetVersion)
{
    std::unique_ptr<MaxOperatorPacket> operatorPacket{ std::make_unique<MaxOperatorPacket>(packetVersion) };
    ReadOperatorSubPackets(bufferReader, *operatorPacket);
    return operatorPacket;
}

std::unique_ptr<GreaterOperatorPacket> ReadGreaterOperator(BufferReader& bufferReader, u64 packetVersion)
{
    std::unique_ptr<GreaterOperatorPacket> operatorPacket{ std::make_unique<GreaterOperatorPacket>(packetVersion) };
    ReadOperatorSubPackets(bufferReader, *operatorPacket);
    return operatorPacket;
}

std::unique_ptr<LessOperatorPacket> ReadLessOperator(BufferReader& bufferReader, u64 packetVersion)
{
    std::unique_ptr<LessOperatorPacket> operatorPacket{ std::make_unique<LessOperatorPacket>(packetVersion) };
    ReadOperatorSubPackets(bufferReader, *operatorPacket);
    return operatorPacket;
}

std::unique_ptr<EqualOperatorPacket> ReadEqualOperator(BufferReader& bufferReader, u64 packetVersion)
{
    std::unique_ptr<EqualOperatorPacket> operatorPacket{ std::make_unique<EqualOperatorPacket>(packetVersion) };
    ReadOperatorSubPackets(bufferReader, *operatorPacket);
    return operatorPacket;
}

std::unique_ptr<Packet> ReadPacket(BufferReader& bufferReader)
{
    std::unique_ptr<Packet> packet{};
    u64 packetVersion{ bufferReader.Read(3) };

    switch ((PacketTypeID)bufferReader.Read(3))
    {
    case PacketTypeID::Sum: { packet = ReadSumOperator(bufferReader, packetVersion); break; }
    case PacketTypeID::Product: { packet = ReadProductOperator(bufferReader, packetVersion); break; }
    case PacketTypeID::Min: { packet = ReadMinOperator(bufferReader, packetVersion); break; }
    case PacketTypeID::Max: { packet = ReadMaxOperator(bufferReader, packetVersion); break; }
    case PacketTypeID::Litteral: { packet = ReadLitteral(bufferReader, packetVersion); break; }
    case PacketTypeID::Greater: { packet = ReadGreaterOperator(bufferReader, packetVersion); break; }
    case PacketTypeID::Less: { packet = ReadLessOperator(bufferReader, packetVersion); break; }
    case PacketTypeID::Equal: { packet = ReadEqualOperator(bufferReader, packetVersion); break; }
    }

    return packet;
}

class AccumulateVersionsPacketVisitor : public PacketVisitor
{
public:
    u64 VisitOperator(const OperatorPacket& operatorPacket)
    {
        u64 total{ operatorPacket.Version };

        for (const auto& subPacket : operatorPacket.SubPackets)
        {
            total += subPacket->Accept(*this);
        }

        return total;
    }

    u64 VisitLitteral(const LitteralPacket& litteralPacket) override
    {
        return litteralPacket.Version;
    }

    u64 VisitSumOperator(const SumOperatorPacket& operatorPacket) override { return VisitOperator(operatorPacket); }
    u64 VisitProductOperator(const ProductOperatorPacket& operatorPacket) override { return VisitOperator(operatorPacket); }
    u64 VisitMinOperator(const MinOperatorPacket& operatorPacket) override { return VisitOperator(operatorPacket); }
    u64 VisitMaxOperator(const MaxOperatorPacket& operatorPacket) override { return VisitOperator(operatorPacket); }
    u64 VisitGreaterOperator(const GreaterOperatorPacket& operatorPacket) override { return VisitOperator(operatorPacket); }
    u64 VisitLessOperator(const LessOperatorPacket& operatorPacket) override { return VisitOperator(operatorPacket); }
    u64 VisitEqualOperator(const EqualOperatorPacket& operatorPacket) override { return VisitOperator(operatorPacket); }
};

class EvaluatePacketVisitor : public PacketVisitor
{
public:
    u64 VisitLitteral(const LitteralPacket& litteralPacket) override
    {
        return litteralPacket.Value;
    }

    u64 VisitSumOperator(const SumOperatorPacket& operatorPacket) override
    {
        u64 result{};
        for (const auto& subPacket : operatorPacket.SubPackets)
        {
            result += subPacket->Accept(*this);
        }
        return result;
    }

    u64 VisitProductOperator(const ProductOperatorPacket& operatorPacket) override
    {
        u64 result{ 1 };
        for (const auto& subPacket : operatorPacket.SubPackets)
        {
            result *= subPacket->Accept(*this);
        }
        return result;
    }

    u64 VisitMinOperator(const MinOperatorPacket& operatorPacket) override
    {
        u64 result{ std::numeric_limits<u64>::max() };
        for (const auto& subPacket : operatorPacket.SubPackets)
        {
            result = std::min(result, subPacket->Accept(*this));
        }
        return result;
    }

    u64 VisitMaxOperator(const MaxOperatorPacket& operatorPacket) override
    {
        u64 result{};
        for (const auto& subPacket : operatorPacket.SubPackets)
        {
            result = std::max(result, subPacket->Accept(*this));
        }
        return result;
    }

    u64 VisitGreaterOperator(const GreaterOperatorPacket& operatorPacket) override
    {
        return operatorPacket.SubPackets[0]->Accept(*this) > operatorPacket.SubPackets[1]->Accept(*this);
    }

    u64 VisitLessOperator(const LessOperatorPacket& operatorPacket) override
    {
        return operatorPacket.SubPackets[0]->Accept(*this) < operatorPacket.SubPackets[1]->Accept(*this);
    }

    u64 VisitEqualOperator(const EqualOperatorPacket& operatorPacket) override
    {
        return operatorPacket.SubPackets[0]->Accept(*this) == operatorPacket.SubPackets[1]->Accept(*this);
    }

};

int main()
{
    BufferReader bufferReader("E20D79005573F71DA0054E48527EF97D3004653BB1FC006867A8B1371AC49C801039171941340066E6B99A6A58B8110088BA008CE6F7893D4E6F7893DCDCFDB9D6CBC4026FE8026200DC7D84B1C00010A89507E3CCEE37B592014D3C01491B6697A83CB4F59E5E7FFA5CC66D4BC6F05D3004E6BB742B004E7E6B3375A46CF91D8C027911797589E17920F4009BE72DA8D2E4523DCEE86A8018C4AD3C7F2D2D02C5B9FF53366E3004658DB0012A963891D168801D08480485B005C0010A883116308002171AA24C679E0394EB898023331E60AB401294D98CA6CD8C01D9B349E0A99363003E655D40289CBDBB2F55D25E53ECAF14D9ABBB4CC726F038C011B0044401987D0BE0C00021B04E2546499DE824C015B004A7755B570013F2DD8627C65C02186F2996E9CCD04E5718C5CBCC016B004A4F61B27B0D9B8633F9344D57B0C1D3805537ADFA21F231C6EC9F3D3089FF7CD25E5941200C96801F191C77091238EE13A704A7CCC802B3B00567F192296259ABD9C400282915B9F6E98879823046C0010C626C966A19351EE27DE86C8E6968F2BE3D2008EE540FC01196989CD9410055725480D60025737BA1547D700727B9A89B444971830070401F8D70BA3B8803F16A3FC2D00043621C3B8A733C8BD880212BCDEE9D34929164D5CB08032594E5E1D25C0055E5B771E966783240220CD19E802E200F4588450BC401A8FB14E0A1805B36F3243B2833247536B70BDC00A60348880C7730039400B402A91009F650028C00E2020918077610021C00C1002D80512601188803B4000C148025010036727EE5AD6B445CC011E00B825E14F4BBF5F97853D2EFD6256F8FFE9F3B001420C01A88915E259002191EE2F4392004323E44A8B4C0069CEF34D304C001AB94379D149BD904507004A6D466B618402477802E200D47383719C0010F8A507A294CC9C90024A967C9995EE2933BA840");
    std::unique_ptr<Packet> rootPacket{ ReadPacket(bufferReader) };

    AccumulateVersionsPacketVisitor versionAccumulator{};
    fmt::print("Total Version: {}\n", rootPacket->Accept(versionAccumulator));

    EvaluatePacketVisitor packetEvaluator{};
    fmt::print("Evaluated Packet Result: {}\n", rootPacket->Accept(packetEvaluator));
    return 0;
}