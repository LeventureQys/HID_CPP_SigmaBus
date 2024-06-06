#pragma once

//�������������ڴ��Maono�ڲ����������б�
#include <iostream>
#include "CRC.h"
namespace HIDParam {
	//0xC4
	const uint8_t Header = 0xC4;			//֡ͷ
	const uint8_t CC_ID = 0x00;				//CC(CP_CMD)����������ʹ��0
	const uint8_t CC_ERROR_CODE = 0x02;		//������Ӧ֡��CMD
	const uint8_t CC_PARAM_RAND_WR = 0x03;	//���д������2�ֽڵ�ַ��
	const uint8_t CC_PARAM_RAND_RD = 0x04;	//�����������2�ֽڵ�ַ��
	const uint8_t CC_PARAM_SEQU_WR = 0x05;  // ����д������2�ֽڵ�ַ��
	const uint8_t CC_PARAM_SEQU_RD = 0x06;  // ������������2�ֽڵ�ַ��

	const uint8_t CC_PARAM_RAND_WR_EXT = 0x07;	//��չ���д������4�ֽڵ�ַ��
	const uint8_t CC_PARAM_RAND_RD_EXT = 0x08;	//��չ�����������4�ֽڵ�ַ��
	const uint8_t CC_PARAM_SEQU_WR_EXT = 0x09;  // ��չ����д������4�ֽڵ�ַ��
	const uint8_t CC_PARAM_SEQU_RD_EXT = 0x0A;  // ��չ������������4�ֽڵ�ַ��

	const uint8_t CC_DFU = 0x0B;				//DFUģʽ �̼���������
	const uint8_t CC_CTRL = 0x0C;			//�߿�ͨѶ����
}

// �����д���ݽṹ
//��ʼ����ͳһʹ��CreateDataRandom����
struct DataRandom {
	uint8_t report;          // ����ID
	uint8_t frame_header;    // ֡ͷ
	uint8_t frame_len_L;     // ֡���ȵ��ֽ�
	uint8_t frame_len_H;     // ֡���ȸ��ֽ�
	uint8_t cp_addr;         // �豸��ַ
	uint8_t command;         // �������
	uint8_t* param_addr;   // ���ݵĵ�ַ,�밴��param1_l,param1_h,param2_l,param2_h�ķ�ʽ����
	uint8_t* param_value;  // ���ݵ�ֵ���밴��param1_l,param1_h,param2_l,param2_h�ķ�ʽ����
	uint8_t CRC_L;
	uint8_t CRC_H;
	// ���ṹ��ת�����ֽ����飬�洢���ⲿ�ṩ�Ļ�������
	inline bool toData(unsigned char* buffer, size_t& length) const;
};

// ��ȡ�����������ṹ��
struct ReadRandomDataPack {
	uint8_t report;          // ����ID
	uint8_t frame_header;    // ֡ͷ
	uint8_t frame_len_L;     // ֡���ȵ��ֽ�
	uint8_t frame_len_H;     // ֡���ȸ��ֽ�
	uint8_t cp_addr;         // �豸��ַ
	uint8_t command;         // �������
	uint8_t* param_addr;     // ���ݵĵ�ַ
	uint8_t CRC_L;           // CRC���ֽ�
	uint8_t CRC_H;           // CRC���ֽ�
	// ���ṹ��ת�����ֽ����飬�洢���ⲿ�ṩ�Ļ�������
	inline bool toData(unsigned char* buffer, size_t& length) const;
};


// ������ַ�Ķ�д����ṹ��
struct DataContinuous {

	uint8_t report;          // ����ID
	uint8_t frame_header;    // ֡ͷ
	uint8_t frame_len_L;     // ֡���ȵ��ֽ�
	uint8_t frame_len_H;     // ֡���ȸ��ֽ�
	uint8_t cp_addr;         // �豸��ַ
	uint8_t command;         // �������
	// ���ݲ���----------------
	uint8_t param_addr[2];   // ���ݵĵ�ַ
	uint8_t param_num[2];    // ���ݵ������򳤶�
	uint8_t* param_value;   // ���ݵ�ֵ���ɱ䳤�ȣ�,�˲������ڴ���Ӧ����CRCǰ��
	uint8_t crc_L;           // CRCУ����ֽ�
	uint8_t crc_H;           // CRCУ����ֽ�
	// ���ṹ��ת�����ֽ����飬�洢���ⲿ�ṩ�Ļ�������
	inline bool toData(unsigned char* buffer, size_t& length) const;
};

// ������ַ�Ķ�ȡ����ṹ��
struct ReadDataContinuous {

	uint8_t report;          // ����ID
	uint8_t frame_header;    // ֡ͷ
	uint8_t frame_len_L;     // ֡���ȵ��ֽ�
	uint8_t frame_len_H;     // ֡���ȸ��ֽ�
	uint8_t cp_addr;         // �豸��ַ
	uint8_t command;         // �������
	// ���ݲ���----------------
	uint8_t param_addr[2];   // ���ݵĵ�ַ
	uint8_t param_num[2];    // ���ݵ������򳤶�
	uint8_t crc_L;           // CRCУ����ֽ�
	uint8_t crc_H;           // CRCУ����ֽ�
	// ���ṹ��ת�����ֽ����飬�洢���ⲿ�ṩ�Ļ�������
	inline bool toData(unsigned char* buffer, size_t& length) const;
};
enum class ReplyType {
	Random = 0x03,			//�����ַ��д
	Continuous = 0x04,		//������ַ��д
	Confirm = 0x02			//ȷ���յ���ϢӦ��
};
bool DataRandom::toData(unsigned char* buffer, size_t& length) const {
	if (!buffer) {
		std::cerr << "Buffer is null!" << std::endl;
		return false;
	}
	
	// ���������ܳ���
	size_t param_size = ((frame_len_H << 8) | frame_len_L) - 7; // ��ȥͷ����CRC�ĳ���
	param_size /= 2;
	length = param_size + 8 + 1; // ͷ��6�ֽ� + param_addr + param_value + CRC

	// ���ṹ����ֶ�д���ֽ�����
	size_t offset = 0;
	buffer[offset++] = report;
	buffer[offset++] = frame_header;
	buffer[offset++] = frame_len_L;
	buffer[offset++] = frame_len_H;
	buffer[offset++] = cp_addr;
	buffer[offset++] = command;

	//����һ���������Ľṹ��С
	int free_param_size = (length - 7) / 4;
	
	for (int i = 0; i < free_param_size; ++i) {
		buffer[offset + 4*i] = param_addr[2*i];
		buffer[offset + (4*i) + 1] = param_addr[(2 * i) + 1];
		buffer[offset + (4 * i) + 2] = param_value[2 * i];
		buffer[offset + (4 * i) + 3] = param_value[(2 * i) + 1];
	}
	offset += 4 * free_param_size;
	// ����CRC
	buffer[offset++] = CRC_L;
	buffer[offset++] = CRC_H;

	return true;
}

bool ReadRandomDataPack::toData(unsigned char* buffer, size_t& length) const {
	if (!buffer) {
		std::cerr << "Buffer is null!" << std::endl;
		return false;
	}

	// ���������ܳ���
	size_t param_size = ((frame_len_H << 8) | frame_len_L) - 8; // ��ȥͷ����CRC�ĳ���
	length = 8 + param_size + 2; // ͷ��6�ֽ� + param_addr + CRC

	// ���ṹ����ֶ�д���ֽ�����
	size_t offset = 0;
	//buffer[offset++] = report;
	buffer[offset++] = frame_header;
	buffer[offset++] = frame_len_L;
	buffer[offset++] = frame_len_H;
	buffer[offset++] = cp_addr;
	buffer[offset++] = command;

	// ����param_addr
	std::memcpy(&buffer[offset], param_addr, param_size);
	offset += param_size;

	// ����CRC
	buffer[offset++] = CRC_L;
	buffer[offset++] = CRC_H;

	return true;
}

bool DataContinuous::toData(unsigned char* buffer, size_t& length) const {
	if (!buffer) {
		std::cerr << "Buffer is null!" << std::endl;
		return false;
	}

	// ���������ܳ���
	size_t param_value_len = ((frame_len_H << 8) | frame_len_L) - 10; // ��ȥͷ���͹̶������ֶ�
	length = 10 + param_value_len + 2; // ͷ��10�ֽ� + param_value + CRC

	// ���ṹ����ֶ�д���ֽ�����
	size_t offset = 0;
	//buffer[offset++] = report;
	buffer[offset++] = frame_header;
	buffer[offset++] = frame_len_L;
	buffer[offset++] = frame_len_H;
	buffer[offset++] = cp_addr;
	buffer[offset++] = command;
	buffer[offset++] = param_addr[0];
	buffer[offset++] = param_addr[1];
	buffer[offset++] = param_num[0];
	buffer[offset++] = param_num[1];

	// ����param_value
	std::memcpy(&buffer[offset], param_value, param_value_len);
	offset += param_value_len;

	// ����CRC
	buffer[offset++] = crc_L;
	buffer[offset++] = crc_H;

	return true;
}

bool ReadDataContinuous::toData(unsigned char* buffer, size_t& length) const {
	if (!buffer) {
		std::cerr << "Buffer is null!" << std::endl;
		return false;
	}

	// ���������ܳ���
	length = 10 + 2; // ͷ��10�ֽ� + CRC

	// ���ṹ����ֶ�д���ֽ�����
	size_t offset = 0;
	//buffer[offset++] = report;
	buffer[offset++] = frame_header;
	buffer[offset++] = frame_len_L;
	buffer[offset++] = frame_len_H;
	buffer[offset++] = cp_addr;
	buffer[offset++] = command;
	buffer[offset++] = param_addr[0];
	buffer[offset++] = param_addr[1];
	buffer[offset++] = param_num[0];
	buffer[offset++] = param_num[1];

	// ����CRC
	buffer[offset++] = crc_L;
	buffer[offset++] = crc_H;

	return true;
}
// ��ʼ��������ʵ��
/**
 * @brief ��ʼ�� DataRandom �ṹ��
 *
 * @param data DataRandom �ṹ��ʵ��
 * @param report ����ID
 * @param frame_len ֡����
 * @param cp_addr �豸��ַ
 * @param command ������룬����Ĭ�ϱ�Ӧ����0x03����ʵ���ϲ�һ��
 * @param param_addr ���ݵĵ�ַ
 * @param param_value ���ݵ�ֵ
 */
inline void CreateDataRandom(DataRandom& data, uint8_t report, uint16_t frame_len, uint8_t cp_addr, uint8_t command, uint8_t* param_addr, uint8_t* param_value, CRCType type) {

	int length = (int)frame_len;

	// ��ջ�����
	std::memset(&data, 0, sizeof(data));
	// ����Э�����ݰ��ṹ
	data.report = report;
	data.frame_header = HIDParam::Header;
	data.frame_len_L = static_cast<uint8_t>(frame_len & 0xFF); // ���ֽ�
	data.frame_len_H = static_cast<uint8_t>((frame_len >> 8) & 0xFF); // ���ֽ�
	data.cp_addr = cp_addr;
	data.command = command;
	data.param_addr = param_addr;
	data.param_value = param_value;

	// ��Ҫ����CRC������ƴ�ӳ�һ������������������ͷ
	uint8_t buffer[128];
	std::memset(buffer, 0, sizeof(buffer));
	buffer[0] = data.frame_header;
	buffer[1] = data.frame_len_L;
	buffer[2] = data.frame_len_H;
	buffer[3] = data.cp_addr;
	buffer[4] = data.command;

	int param_size = (length - 7) / 4;

	for (int i = 0; i < param_size; ++i) {
		buffer[5 + i * 4] = param_addr[2*i];
		buffer[6 + i * 4] = param_addr[2 * i + 1];
		buffer[7 + i * 4] = param_value[2 * i];
		buffer[8 + i * 4] = param_value[2 * i + 1];
	}

	uint32_t crc = CCRC::Crc(buffer, length - 1, type);

	// ��CRC��Ϊ���ֽں͸��ֽ�
	data.CRC_L = static_cast<uint8_t>(crc & 0xFF);
	data.CRC_H = static_cast<uint8_t>((crc >> 8) & 0xFF);
}

// ���� DataRandom �ṹ��ĺ���
inline bool ParseDataRandom(const unsigned char* data, size_t length, DataRandom& dataRandom) {
	if (length < 10) { // �����ṹ��������Ϊ10�ֽ�
		std::cerr << "Data length too short to parse DataRandom structure." << std::endl;
		return false;
	}
	// ��ջ�����
	std::memset(&dataRandom, 0, sizeof(dataRandom));
	size_t offset = 0;

	// ��ȡЭ�����ݰ��ṹ���ֶ�
	dataRandom.report = data[offset++];
	dataRandom.frame_header = data[offset++];
	dataRandom.frame_len_L = data[offset++];
	dataRandom.frame_len_H = data[offset++];
	dataRandom.cp_addr = data[offset++];
	dataRandom.command = data[offset++];

	// ����֡���ݳ���
	uint16_t frame_len = (static_cast<uint16_t>(dataRandom.frame_len_H) << 8) | static_cast<uint16_t>(dataRandom.frame_len_L);
	if (length < frame_len) {
		std::cerr << "Data length mismatch with frame length." << std::endl;
		return false;
	}

	// ��̬�����ڴ沢�������ݵ� param_addr �� param_value
	size_t param_size = frame_len - 8; // ��ȥͷ����CRC�ĳ���
	dataRandom.param_addr = new uint8_t[param_size];
	dataRandom.param_value = new uint8_t[param_size];

	std::memcpy(dataRandom.param_addr, &data[offset], param_size);
	offset += param_size;
	std::memcpy(dataRandom.param_value, &data[offset], param_size);
	offset += param_size;

	// ��ȡ CRC
	dataRandom.CRC_L = data[offset++];
	dataRandom.CRC_H = data[offset++];

	return true;
}



// �ͷ�myrandom_write�ṹ��ʵ��
inline void FreeDataRandom(DataRandom* packet) {
	free(packet);
}



// ���� ReadRandomDataPack �ṹ��ĺ���
inline void CreateReadRandomDataPack(ReadRandomDataPack& data, uint8_t report, uint16_t frame_len, uint8_t cp_addr, uint8_t command, uint8_t* param_addr, CRCType type) {
	// ���������Ϊ0�����Ը���ʵ����Ҫ����CRCֵ
   //�����Ҫ����CRC������Ҫ����������ȡ��������
	int length = (int)frame_len;

	//��ָ�����򳤶�
	length -= 8;

	//����������ֱ���˳�
	if (length % 4 != 0 || length / 4 == 0) return;

	// ��ջ�����
	std::memset(&data, 0, sizeof(data));
	// ����Э�����ݰ��ṹ
	data.frame_header = HIDParam::Header;
	data.frame_len_L = static_cast<uint8_t>(frame_len & 0xFF); // ���ֽ�
	data.frame_len_H = static_cast<uint8_t>((frame_len >> 8) & 0xFF); // ���ֽ�
	data.cp_addr = cp_addr;
	data.command = command;
	data.param_addr = param_addr;

	// ��Ҫ����CRC������ƴ�ӳ�һ������������������ͷ
	uint8_t buffer[128];
	std::memset(buffer, 0, sizeof(buffer));
	buffer[0] = data.frame_header;
	buffer[1] = data.frame_len_L;
	buffer[2] = data.frame_len_H;
	buffer[3] = data.cp_addr;
	buffer[4] = data.command;
	std::memcpy(&buffer[5], param_addr, length - 8); // ��ȥͷ���ֶκ�CRC�ֶεĳ���

	uint32_t crc = CCRC::Crc(buffer, length - 1, type);

	// ��CRC��Ϊ���ֽں͸��ֽ�
	data.CRC_L = static_cast<uint8_t>(crc & 0xFF);
	data.CRC_H = static_cast<uint8_t>((crc >> 8) & 0xFF);
}







inline void CreateDataContinuous(DataContinuous& data, uint8_t report, uint16_t frame_len, uint8_t cp_addr, uint8_t command, uint8_t* param_addr, uint8_t* param_num, uint8_t* param_value, uint16_t param_value_len) {
	// ��ջ�����
	std::memset(&data, 0, sizeof(data));

	// ����Э�����ݰ��ṹ
	data.report = report;
	data.frame_header = HIDParam::Header;
	data.frame_len_L = static_cast<uint8_t>(frame_len & 0xFF); // ���ֽ�
	data.frame_len_H = static_cast<uint8_t>((frame_len >> 8) & 0xFF); // ���ֽ�
	data.cp_addr = cp_addr;
	data.command = command;
	std::memcpy(data.param_addr, param_addr, 2);
	std::memcpy(data.param_num, param_num, 2);

	// ��̬���䲢���� param_value
	data.param_value = new uint8_t[param_value_len];
	std::memcpy(data.param_value, param_value, param_value_len);

	// ��Ҫ����CRC������ƴ�ӳ�һ��������
	uint8_t* buffer = new uint8_t[6 + 2 + 2 + param_value_len]; // ��̬���仺����
	size_t offset = 0;
	buffer[offset++] = data.frame_header;
	buffer[offset++] = data.frame_len_L;
	buffer[offset++] = data.frame_len_H;
	buffer[offset++] = data.cp_addr;
	buffer[offset++] = data.command;
	std::memcpy(&buffer[offset], data.param_addr, 2);
	offset += 2;
	std::memcpy(&buffer[offset], data.param_num, 2);
	offset += 2;
	std::memcpy(&buffer[offset], data.param_value, param_value_len);
	offset += param_value_len;

	// ����CRC
	uint32_t crc = CCRC::Crc32(buffer, offset);

	// ��CRC��Ϊ���ֽں͸��ֽ�
	data.crc_L = static_cast<uint8_t>(crc & 0xFF);
	data.crc_H = static_cast<uint8_t>((crc >> 8) & 0xFF);

	// �ͷŶ�̬����Ļ�����
	
}

inline bool ParseDataContinuous(const unsigned char* data, size_t length, DataContinuous& dataContinuous) {
	if (length < 10) { // �����ṹ��������Ϊ10�ֽڣ��������ɱ䳤�ȵ�����
		std::cerr << "Data length too short to parse DataContinuous structure." << std::endl;
		return false;
	}

	size_t offset = 0;

	// ��ȡЭ�����ݰ��ṹ���ֶ�
	dataContinuous.report = data[offset++];
	dataContinuous.frame_header = data[offset++];
	dataContinuous.frame_len_L = data[offset++];
	dataContinuous.frame_len_H = data[offset++];
	dataContinuous.cp_addr = data[offset++];
	dataContinuous.command = data[offset++];

	// ��ȡ��ַ�������ֶ�
	std::memcpy(dataContinuous.param_addr, &data[offset], 2);
	offset += 2;
	std::memcpy(dataContinuous.param_num, &data[offset], 2);
	offset += 2;

	// ����֡���ݳ���
	uint16_t frame_len = (static_cast<uint16_t>(dataContinuous.frame_len_H) << 8) | static_cast<uint16_t>(dataContinuous.frame_len_L);
	if (length < frame_len) {
		std::cerr << "Data length mismatch with frame length." << std::endl;
		return false;
	}

	// ����ɱ䳤�ȵ����ݲ���
	size_t param_value_len = frame_len - 10; // ��ȥͷ���͹̶������ֶ�

	// ��̬�����ڴ沢�������ݵ� param_value
	dataContinuous.param_value = new uint8_t[param_value_len];
	std::memcpy(dataContinuous.param_value, &data[offset], param_value_len);
	offset += param_value_len;

	// ��ȡ CRC
	dataContinuous.crc_L = data[offset++];
	dataContinuous.crc_H = data[offset++];

	return true;
}

// �ͷ� DataContinuous �ṹ��Ķ�̬�ڴ�
inline void FreeDataContinuous(DataContinuous& dataContinuous) {
	delete[] dataContinuous.param_value;
	dataContinuous.param_value = nullptr;
}





inline void CreateReadDataContinuous(ReadDataContinuous& data, uint8_t report, uint16_t frame_len, uint8_t cp_addr, uint8_t command, uint8_t* param_addr, uint8_t* param_num) {
	// ��ջ�����
	std::memset(&data, 0, sizeof(data));

	// ����Э�����ݰ��ṹ
	data.report = report;
	data.frame_header = 0x02; // ʾ��֡ͷ�����Ը�����Ҫ�޸�
	data.frame_len_L = static_cast<uint8_t>(frame_len & 0xFF); // ���ֽ�
	data.frame_len_H = static_cast<uint8_t>((frame_len >> 8) & 0xFF); // ���ֽ�
	data.cp_addr = cp_addr;
	data.command = command;
	std::memcpy(data.param_addr, param_addr, 2);
	std::memcpy(data.param_num, param_num, 2);

	// ����CRC
	uint8_t buffer[10];
	buffer[0] = data.report;
	buffer[1] = data.frame_header;
	buffer[2] = data.frame_len_L;
	buffer[3] = data.frame_len_H;
	buffer[4] = data.cp_addr;
	buffer[5] = data.command;
	buffer[6] = data.param_addr[0];
	buffer[7] = data.param_addr[1];
	buffer[8] = data.param_num[0];
	buffer[9] = data.param_num[1];

	uint32_t crc = CCRC::Crc32(buffer, 10);

	// ��CRC��Ϊ���ֽں͸��ֽ�
	data.crc_L = static_cast<uint8_t>(crc & 0xFF);
	data.crc_H = static_cast<uint8_t>((crc >> 8) & 0xFF);
}
