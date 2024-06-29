#pragma once

//常见参数表，用于存放Maono内部常见参数列表
#include <iostream>
#include "CRC.h"
namespace HIDParam {
	//0xC4
	const uint8_t Header = 0xC4;			//帧头
	const uint8_t CC_ID = 0x00;				//CC(CP_CMD)保留，不能使用0
	const uint8_t CC_ERROR_CODE = 0x02;		//错误响应帧的CMD
	const uint8_t CC_PARAM_RAND_WR = 0x03;	//随机写参数（2字节地址）
	const uint8_t CC_PARAM_RAND_RD = 0x04;	//随机读参数（2字节地址）
	const uint8_t CC_PARAM_SEQU_WR = 0x05;  // 连续写参数（2字节地址）
	const uint8_t CC_PARAM_SEQU_RD = 0x06;  // 连续读参数（2字节地址）

	const uint8_t CC_PARAM_RAND_WR_EXT = 0x07;	//扩展随机写参数（4字节地址）
	const uint8_t CC_PARAM_RAND_RD_EXT = 0x08;	//扩展随机读参数（4字节地址）
	const uint8_t CC_PARAM_SEQU_WR_EXT = 0x09;  // 扩展连续写参数（4字节地址）
	const uint8_t CC_PARAM_SEQU_RD_EXT = 0x0A;  // 扩展连续读参数（4字节地址）

	const uint8_t CC_DFU = 0x0B;				//DFU模式 固件升级命令
	const uint8_t CC_CTRL = 0x0C;			//线控通讯命令
}

// 随机写数据结构
//初始化请统一使用CreateDataRandom进行
struct DataRandom {
	uint8_t report;          // 报文ID
	uint8_t frame_header;    // 帧头
	uint8_t frame_len_L;     // 帧长度低字节
	uint8_t frame_len_H;     // 帧长度高字节
	uint8_t cp_addr;         // 设备地址
	uint8_t command;         // 命令编码
	uint8_t* param_addr;   // 数据的地址,请按照param1_l,param1_h,param2_l,param2_h的方式排列
	uint8_t* param_value;  // 数据的值，请按照param1_l,param1_h,param2_l,param2_h的方式排列
	uint8_t CRC_L;
	uint8_t CRC_H;
	// 将结构体转换成字节数组，存储在外部提供的缓冲区中
	inline bool toData(unsigned char* buffer, size_t& length) const;
};

// 读取随机参数请求结构体
struct ReadRandomDataPack {
	uint8_t report;          // 报文ID
	uint8_t frame_header;    // 帧头
	uint8_t frame_len_L;     // 帧长度低字节
	uint8_t frame_len_H;     // 帧长度高字节
	uint8_t cp_addr;         // 设备地址
	uint8_t command;         // 命令编码
	uint8_t* param_addr;     // 数据的地址
	uint8_t CRC_L;           // CRC低字节
	uint8_t CRC_H;           // CRC高字节
	// 将结构体转换成字节数组，存储在外部提供的缓冲区中
	inline bool toData(unsigned char* buffer, size_t& length) const;
};


// 连续地址的写请求结构体
struct DataContinuous {

	uint8_t report;          // 报文ID
	uint8_t frame_header;    // 帧头
	uint8_t frame_len_L;     // 帧长度低字节
	uint8_t frame_len_H;     // 帧长度高字节
	uint8_t cp_addr;         // 设备地址
	uint8_t command;         // 命令编码
	// 数据部分----------------
	uint8_t param_addr[2];   // 数据的地址
	uint8_t param_num[2];    // 数据的数量或长度
	uint8_t* param_value;   // 数据的值（可变长度）,此部分在内存上应属于CRC前方
	uint8_t crc_L;           // CRC校验低字节
	uint8_t crc_H;           // CRC校验高字节
	// 将结构体转换成字节数组，存储在外部提供的缓冲区中
	inline bool toData(unsigned char* buffer, size_t& length) const;
};

// 连续地址的读取请求结构体
struct ReadDataContinuous {

	uint8_t report;          // 报文ID
	uint8_t frame_header;    // 帧头
	uint8_t frame_len_L;     // 帧长度低字节
	uint8_t frame_len_H;     // 帧长度高字节
	uint8_t cp_addr;         // 设备地址
	uint8_t command;         // 命令编码
	// 数据部分----------------
	uint8_t param_addr[2];   // 数据的地址
	uint8_t param_num[2];    // 数据的数量或长度
	uint8_t crc_L;           // CRC校验低字节
	uint8_t crc_H;           // CRC校验高字节
	// 将结构体转换成字节数组，存储在外部提供的缓冲区中
	inline bool toData(unsigned char* buffer, size_t& length) const;
};

//应答帧
struct ReplyFrame {
	uint8_t report;		//报头ID
	uint8_t frame_header;	//帧头
	uint8_t frame_len_L;     // 帧长度低字节
	uint8_t frame_len_H;     // 帧长度高字节
	uint8_t cp_addr;         // 设备地址
	uint8_t command;         // 命令编码
	uint8_t ERR_CODE_L;      // 错误代码低字节
	uint8_t ERR_CODE_H;      // 错误代码高字节
	uint8_t CRC_L;           // CRC校验低字节
	uint8_t CRC_H;           // CRC校验高字节
	inline bool FromData(unsigned char* buffer, size_t length) {
		if (length < 9) {
			std::cerr << "Invalid data length!" << std::endl;
			return false;
		}

		if (length == 9) {
			// 无报头的情况
			frame_header = buffer[0];
			frame_len_L = buffer[1];
			frame_len_H = buffer[2];
			cp_addr = buffer[3];
			command = buffer[4];
			ERR_CODE_L = buffer[5];
			ERR_CODE_H = buffer[6];
			CRC_L = buffer[7];
			CRC_H = buffer[8];
		}
		else if (length == 10) {
			// 有报头的情况
			report = buffer[0];
			frame_header = buffer[1];
			frame_len_L = buffer[2];
			frame_len_H = buffer[3];
			cp_addr = buffer[4];
			command = buffer[5];
			ERR_CODE_L = buffer[6];
			ERR_CODE_H = buffer[7];
			CRC_L = buffer[8];
			CRC_H = buffer[9];
		}
		else {
			std::cerr << "Unexpected data length!" << std::endl;
			return false;
		}
		return true;
	}
};
enum class ReplyType {
	Continuous = 0x06,			//连续地址读
	Random = 0x04,		//随机地址读
	Confirm = 0x02			//接到写入信息后的标准应答
};
bool DataRandom::toData(unsigned char* buffer, size_t& length) const {
	if (!buffer) {
		std::cerr << "Buffer is null!" << std::endl;
		return false;
	}

	// 计算数据总长度
	size_t param_size = ((frame_len_H << 8) | frame_len_L) - 7; // 除去头部和CRC的长度
	param_size /= 2;
	length = param_size + 8 + 1; // 头部6字节 + param_addr + param_value + CRC

	// 将结构体的字段写入字节数组
	size_t offset = 0;
	buffer[offset++] = report;
	buffer[offset++] = frame_header;
	buffer[offset++] = frame_len_L;
	buffer[offset++] = frame_len_H;
	buffer[offset++] = cp_addr;
	buffer[offset++] = command;

	//计算一下自由区的结构大小
	int free_param_size = (length - 7) / 4;

	for (int i = 0; i < free_param_size; ++i) {
		buffer[offset + 4 * i] = param_addr[2 * i];
		buffer[offset + (4 * i) + 1] = param_addr[(2 * i) + 1];
		buffer[offset + (4 * i) + 2] = param_value[2 * i];
		buffer[offset + (4 * i) + 3] = param_value[(2 * i) + 1];
	}
	offset += 4 * free_param_size;
	// 复制CRC
	buffer[offset++] = CRC_L;
	buffer[offset++] = CRC_H;

	return true;
}

bool ReadRandomDataPack::toData(unsigned char* buffer, size_t& length) const {
	if (!buffer) {
		std::cerr << "Buffer is null!" << std::endl;
		return false;
	}

	// 计算数据总长度
	size_t param_size = ((frame_len_H << 8) | frame_len_L) - 8; // 除去头部和CRC的长度
	length = 8 + param_size + 2; // 头部6字节 + param_addr + CRC

	// 将结构体的字段写入字节数组
	size_t offset = 0;
	//buffer[offset++] = report;
	buffer[offset++] = frame_header;
	buffer[offset++] = frame_len_L;
	buffer[offset++] = frame_len_H;
	buffer[offset++] = cp_addr;
	buffer[offset++] = command;

	// 复制param_addr
	std::memcpy(&buffer[offset], param_addr, param_size);
	offset += param_size;

	// 复制CRC
	buffer[offset++] = CRC_L;
	buffer[offset++] = CRC_H;

	return true;
}

bool DataContinuous::toData(unsigned char* buffer, size_t& length) const {
	if (!buffer) {
		std::cerr << "Buffer is null!" << std::endl;
		return false;
	}

	// 计算数据总长度
	size_t param_value_len = ((frame_len_H << 8) | frame_len_L) - 10; // 除去头部和固定长度字段
	length = 10 + param_value_len + 2; // 头部10字节 + param_value + CRC

	// 将结构体的字段写入字节数组
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

	// 复制param_value
	std::memcpy(&buffer[offset], param_value, param_value_len);
	offset += param_value_len;

	// 复制CRC
	buffer[offset++] = crc_L;
	buffer[offset++] = crc_H;

	return true;
}

bool ReadDataContinuous::toData(unsigned char* buffer, size_t& length) const {
	if (!buffer) {
		std::cerr << "Buffer is null!" << std::endl;
		return false;
	}

	// 计算数据总长度
	length = 10 + 2; // 头部10字节 + CRC

	// 将结构体的字段写入字节数组
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

	// 复制CRC
	buffer[offset++] = crc_L;
	buffer[offset++] = crc_H;

	return true;
}
// 初始化函数的实现
/**
 * @brief 初始化 DataRandom 结构体
 *
 * @param data DataRandom 结构体实例
 * @param report 报文ID
 * @param frame_len 帧长度
 * @param cp_addr 设备地址
 * @param command 命令编码，这里默认本应该是0x03，但实际上不一定
 * @param param_addr 数据的地址
 * @param param_value 数据的值
 */
inline void CreateDataRandom(DataRandom& data, uint8_t report, uint16_t frame_len, uint8_t cp_addr, uint8_t command, uint8_t* param_addr, uint8_t* param_value, CRCType type) {

	int length = (int)frame_len;

	// 清空缓冲区
	std::memset(&data, 0, sizeof(data));
	// 设置协议数据包结构
	data.report = report;
	data.frame_header = HIDParam::Header;
	data.frame_len_L = static_cast<uint8_t>(frame_len & 0xFF); // 低字节
	data.frame_len_H = static_cast<uint8_t>((frame_len >> 8) & 0xFF); // 高字节
	data.cp_addr = cp_addr;
	data.command = command;
	data.param_addr = param_addr;
	data.param_value = param_value;

	// 将要计算CRC的数据拼接成一个缓冲区，不包括报头
	uint8_t buffer[128];
	std::memset(buffer, 0, sizeof(buffer));
	buffer[0] = data.frame_header;
	buffer[1] = data.frame_len_L;
	buffer[2] = data.frame_len_H;
	buffer[3] = data.cp_addr;
	buffer[4] = data.command;

	int param_size = (length - 7) / 4;

	for (int i = 0; i < param_size; ++i) {
		buffer[5 + i * 4] = param_addr[2 * i];
		buffer[6 + i * 4] = param_addr[2 * i + 1];
		buffer[7 + i * 4] = param_value[2 * i];
		buffer[8 + i * 4] = param_value[2 * i + 1];
	}

	uint32_t crc = CCRC::Crc(buffer, length - 1, type);

	// 将CRC分为低字节和高字节
	data.CRC_L = static_cast<uint8_t>(crc & 0xFF);
	data.CRC_H = static_cast<uint8_t>((crc >> 8) & 0xFF);
}

// 解析 DataRandom 结构体的函数
inline bool ParseDataRandom(const unsigned char* data, size_t length, DataRandom& dataRandom) {
	if (length < 10) { // 基本结构长度至少为10字节
		std::cerr << "Data length too short to parse DataRandom structure." << std::endl;
		return false;
	}
	// 清空缓冲区
	std::memset(&dataRandom, 0, sizeof(dataRandom));
	size_t offset = 0;
	//report id不参与crc计算
	// 提取协议数据包结构的字段
	dataRandom.report = data[offset++];
	dataRandom.frame_header = data[offset++];
	dataRandom.frame_len_L = data[offset++];
	dataRandom.frame_len_H = data[offset++];
	dataRandom.cp_addr = data[offset++];
	dataRandom.command = data[offset++];



	// 计算帧数据长度
	uint16_t frame_len = (static_cast<uint16_t>(dataRandom.frame_len_H) << 8) | static_cast<uint16_t>(dataRandom.frame_len_L) + 1;
	if (length < frame_len) {
		std::cerr << "Data length mismatch with frame length." << std::endl;
		return false;
	}

	// 动态分配内存并复制数据到 param_addr 和 param_value
	size_t param_size = (frame_len - 8) / 2; // 除去头部和CRC的长度
	dataRandom.param_addr = new uint8_t[param_size];
	dataRandom.param_value = new uint8_t[param_size];

	std::memcpy(dataRandom.param_addr, &data[offset], param_size);
	offset += param_size;
	std::memcpy(dataRandom.param_value, &data[offset], param_size);
	offset += param_size;


	//创建一个内存数组，用来计算CRC数据


	// 提取 CRC
	dataRandom.CRC_L = data[offset++];
	dataRandom.CRC_H = data[offset++];
	uint16_t ret = (static_cast<uint16_t>(dataRandom.CRC_H) << 8) | dataRandom.CRC_L;
	//获取CRC并进行校验比对，使用crc16进行计算
	int int_crc_size = frame_len - 3;
	uint8_t* crc_buffer = new uint8_t[int_crc_size];

	const uint8_t* new_start = data + 1;
	std::memcpy(crc_buffer, new_start, int_crc_size);


	//计算crc

	uint16_t crc_ret = CCRC::Crc16(crc_buffer, int_crc_size);

	if (ret == crc_ret) {
		return true;
	}
	else {
		//如果这里失败了，需要打印结果
		std::cout << __FUNCTION__ << "crc failed";
		return false;
	}


	return false;
}

// 释放myrandom_write结构体实例
inline void FreeDataRandom(DataRandom* packet) {
	free(packet);
}

// 创建 ReadRandomDataPack 结构体的函数
inline void CreateReadRandomDataPack(ReadRandomDataPack& data, uint8_t report, uint16_t frame_len, uint8_t cp_addr, uint8_t command, uint8_t* param_addr, CRCType type) {
	// 这里简单设置为0，可以根据实际需要计算CRC值
   //如果需要计算CRC，则需要把所有数据取出来计算
	int length = (int)frame_len;

	//非指针区域长度
	length -= 8;

	//长度有误则直接退出
	if (length % 4 != 0 || length / 4 == 0) return;

	// 清空缓冲区
	std::memset(&data, 0, sizeof(data));
	// 设置协议数据包结构
	data.frame_header = HIDParam::Header;
	data.frame_len_L = static_cast<uint8_t>(frame_len & 0xFF); // 低字节
	data.frame_len_H = static_cast<uint8_t>((frame_len >> 8) & 0xFF); // 高字节
	data.cp_addr = cp_addr;
	data.command = command;
	data.param_addr = param_addr;

	// 将要计算CRC的数据拼接成一个缓冲区，不包括报头
	uint8_t buffer[128];
	std::memset(buffer, 0, sizeof(buffer));
	buffer[0] = data.frame_header;
	buffer[1] = data.frame_len_L;
	buffer[2] = data.frame_len_H;
	buffer[3] = data.cp_addr;
	buffer[4] = data.command;
	std::memcpy(&buffer[5], param_addr, length - 8); // 减去头部字段和CRC字段的长度

	uint32_t crc = CCRC::Crc(buffer, length - 1, type);

	// 将CRC分为低字节和高字节
	data.CRC_L = static_cast<uint8_t>(crc & 0xFF);
	data.CRC_H = static_cast<uint8_t>((crc >> 8) & 0xFF);
}

inline void CreateDataContinuous(DataContinuous& data, uint8_t report, uint16_t frame_len, uint8_t cp_addr, uint8_t command, uint8_t* param_addr, uint8_t* param_num, uint8_t* param_value, uint16_t param_value_len) {
	// 清空缓冲区
	std::memset(&data, 0, sizeof(data));
	int int_frame_len = frame_len;
	// 设置协议数据包结构
	data.report = report;
	data.frame_header = HIDParam::Header;
	data.frame_len_L = static_cast<uint8_t>(frame_len & 0xFF); // 低字节
	data.frame_len_H = static_cast<uint8_t>((frame_len >> 8) & 0xFF); // 高字节
	data.cp_addr = cp_addr;
	data.command = command;
	std::memcpy(data.param_addr, param_addr, 2);
	std::memcpy(data.param_num, param_num, 2);

	// 动态分配并复制 param_value
	data.param_value = new uint8_t[param_value_len];
	std::memcpy(data.param_value, param_value, param_value_len);

	// 将要计算CRC的数据拼接成一个缓冲区
	uint8_t* buffer = new uint8_t[6 + 2 + 2 + param_value_len]; // 动态分配缓冲区
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

	// 计算CRC
	uint16_t crc = CCRC::Crc16(buffer, offset);

	// 将CRC分为低字节和高字节
	data.crc_L = static_cast<uint8_t>(crc & 0xFF);
	data.crc_H = static_cast<uint8_t>((crc >> 8) & 0xFF);
}

inline bool ParseDataContinuous(const unsigned char* data, size_t length, DataContinuous& dataContinuous) {
	if (length < 10) { // 基本结构长度至少为10字节，不包含可变长度的数据
		std::cerr << "Data length too short to parse DataContinuous structure." << std::endl;
		return false;
	}

	size_t offset = 0;

	// 提取协议数据包结构的字段
	dataContinuous.report = data[offset++];
	dataContinuous.frame_header = data[offset++];
	dataContinuous.frame_len_L = data[offset++];
	dataContinuous.frame_len_H = data[offset++];
	dataContinuous.cp_addr = data[offset++];
	dataContinuous.command = data[offset++];

	// 提取地址和数量字段
	std::memcpy(dataContinuous.param_addr, &data[offset], 2);
	offset += 2;
	std::memcpy(dataContinuous.param_num, &data[offset], 2);
	offset += 2;

	// 计算帧数据长度
	uint16_t frame_len = (static_cast<uint16_t>(dataContinuous.frame_len_H) << 8) | static_cast<uint16_t>(dataContinuous.frame_len_L) + 1;
	if (length < frame_len) {
		std::cerr << "Data length mismatch with frame length." << std::endl;
		return false;
	}

	// 计算可变长度的数据部分
	size_t param_value_len = frame_len - 8; // 除去头部和固定长度字段

	// 动态分配内存并复制数据到 param_value
	dataContinuous.param_value = new uint8_t[param_value_len];
	std::memcpy(dataContinuous.param_value, &data[offset], param_value_len);
	offset += param_value_len;

	// 提取 CRC
	dataContinuous.crc_L = data[offset++];
	dataContinuous.crc_H = data[offset++];

	uint16_t ret_crc = (dataContinuous.crc_H << 8) | dataContinuous.crc_L;


	//这里需要进行CRC校验，用以返回true or false
	const uint8_t* newStart = data + 1;
	uint8_t* crc_buffer = new uint8_t(frame_len - 3);
	std::memcpy(crc_buffer, newStart, frame_len - 3);
	uint16_t ret = CCRC::Crc16(crc_buffer, frame_len - 3);
	if (ret == ret_crc) {
		return true;
	}
	else {
		return false;
	}
	return false;
}

// 释放 DataContinuous 结构体的动态内存
inline void FreeDataContinuous(DataContinuous& dataContinuous) {
	delete[] dataContinuous.param_value;
	dataContinuous.param_value = nullptr;
}


inline void CreateReadDataContinuous(ReadDataContinuous& data, uint8_t report, uint16_t frame_len, uint8_t cp_addr, uint8_t command, uint8_t* param_addr, uint8_t* param_num) {
	// 清空缓冲区
	std::memset(&data, 0, sizeof(data));

	// 设置协议数据包结构
	data.report = report;
	data.frame_header = 0x02; // 示例帧头，可以根据需要修改
	data.frame_len_L = static_cast<uint8_t>(frame_len & 0xFF); // 低字节
	data.frame_len_H = static_cast<uint8_t>((frame_len >> 8) & 0xFF); // 高字节
	data.cp_addr = cp_addr;
	data.command = command;
	std::memcpy(data.param_addr, param_addr, 2);
	std::memcpy(data.param_num, param_num, 2);

	// 计算CRC
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

	uint16_t crc = CCRC::Crc16(buffer, 10);

	// 将CRC分为低字节和高字节
	data.crc_L = static_cast<uint8_t>(crc & 0xFF);
	data.crc_H = static_cast<uint8_t>((crc >> 8) & 0xFF);
}
