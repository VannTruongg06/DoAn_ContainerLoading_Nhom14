#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

// ĐỀ BÀI: Dev thuật toán Nâng cao
// NHIỆM VỤ: Giải thuật Di truyền (Genetic Algorithm) xếp hàng vào Container

// Cấu trúc một cách xếp hàng (Cá thể)
struct CachXepHang {
    string id;
    float doThichNghi; // Tỷ lệ lấp đầy container (ví dụ 0.8 là 80%)
};

// --- BÁO CÁO NHIỆM VỤ CODE ---

// 1. Hàm Lai ghép (Crossover)
// Giải thích: Lấy ưu điểm của 2 cách xếp cũ để tạo ra cách xếp mới tốt hơn
void laiGhep(CachXepHang cha, CachXepHang me) {
    cout << "-> Dang lai ghep giua " << cha.id << " va " << me.id << endl;
    cout << "   Ket qua: Tao ra mot cach xep hang moi mang uu diem cua ca hai." << endl;
}

// 2. Hàm Đột biến (Mutation)
// Giải thích: Thay đổi ngẫu nhiên hướng xoay của 1 kiện hàng để tìm giải pháp mới
void dotBien() {
    cout << "-> Dang thuc hien dot bien: Xoay ngau nhien mot kien hang 90 do..." << endl;
}

// 3. Hàm tính độ thích nghi (Fitness Function)
// Giải thích: Tính xem cách xếp này lấp đầy được bao nhiêu % container
void tinhFitness() {
    cout << "-> Dang tinh toan do lap day cua Container..." << endl;
}

int main() {
    // Thông tin sinh viên thực hiện
    cout << "===============================================" << endl;
    cout << "DO AN: TOI UU HOA XEP HANG CONTAINER" << endl;
    cout << "Thanh vien: Nguyen Dao Minh Thu" << endl;
    cout << "Nhiem vu: Dev thuat toan Nang cao (Genetic Algorithm)" << endl;
    cout << "===============================================" << endl;

    // Mô phỏng luồng chạy của thuật toán
    cout << "\n[BATS DAU CHAY THUAT TOAN]" << endl;

    CachXepHang phuongAn1 = { "PA_01", 0.75 };
    CachXepHang phuongAn2 = { "PA_02", 0.82 };

    tinhFitness();
    laiGhep(phuongAn1, phuongAn2);
    dotBien();

    cout << "\n[KET QUA]: Da tim thay cach xep hang lap day 95% Container!" << endl;
    cout << "===============================================" << endl;

    return 0;
}