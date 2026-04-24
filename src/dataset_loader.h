#ifndef DATASET_LOADER_H
#define DATASET_LOADER_H

#include "models.h"
#include <string>
#include <vector>

/**
 * @class DatasetLoader
 * @brief Lớp tiện ích hỗ trợ đọc dữ liệu chuẩn hóa từ các file Text/Dataset
 * 
 * Lớp này được thiết kế chuyên nghiệp (C++ Pro) để TV2/TV3 dễ dàng tái sử dụng
 * khi cần load dữ liệu từ các bộ test sinh ra bởi Data Generator hoặc Parser (ORLib, BED-BPP, ESICUP).
 */
class DatasetLoader {
public:
    /**
     * @brief Đọc thông số Container (Kích thước, Trọng lượng tối đa) từ Header của file.
     * @param filename Đường dẫn tới file dataset (.txt)
     * @return Đối tượng Container được khởi tạo theo thông số trong file.
     */
    static Container loadContainer(const std::string& filename);

    /**
     * @brief Đọc danh sách các kiện hàng (Items) từ file.
     * @param filename Đường dẫn tới file dataset (.txt)
     * @return std::vector<Item> danh sách các kiện hàng đã parse thành công.
     */
    static std::vector<Item> loadItems(const std::string& filename);
};

#endif // DATASET_LOADER_H
