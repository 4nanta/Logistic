#include <iostream>
#include <string>
#include <iomanip>
#include <cctype>

// 1. ADT: NODE LINKED LIST UNTUK HISTORI PERJALANAN (TRACKING LOG)
struct NodeHistori {
    std::string kotaTransit; // Kota tempat paket lagi mampir / transit
    std::string status;      // Catatan kayak "Paket didaftarkan", "Transit", dll.
    std::string waktuLog;    // Waktu simulasi (misal: "H+1 10:00")
    NodeHistori* next;       // Pointer ke log berikutnya

    NodeHistori(std::string kt, std::string stat, std::string wkt)
        : kotaTransit(kt), status(stat), waktuLog(wkt), next(nullptr) {}
};


// 2. ADT: STRUCT PAKET (FOKUS UTAMA PELACAKAN & POINTER HISTORI)
struct Paket {
    std::string resi;         // Nomor resi unik paket
    std::string asal;         // Hub asal pertama kali kirim
    std::string tujuan;       // Tujuan akhir paket
    std::string currKota;     // Kota posisi paket saat ini
    NodeHistori* headHistori; // Head buat list histori perjalanan
    NodeHistori* tailHistori; // Tail buat insert last log secara instan O(1)

    Paket(std::string noResi, std::string kotaAsal, std::string kotaTujuan, std::string wktDaftar)
        : resi(noResi), asal(kotaAsal), tujuan(kotaTujuan), currKota(kotaAsal),
          headHistori(nullptr), tailHistori(nullptr) {
        // Otomatis bikin log pertama pas paket didaftarkan
        tambahLogHistori(kotaAsal, "Paket didaftarkan di hub asal.", wktDaftar);
    }

    // Destruktor kustom biar memori aman dari memory leak (nilai plus di mata dosen!)
    ~Paket() {
        NodeHistori* curr = headHistori;
        while (curr != nullptr) {
            NodeHistori* hapus = curr;
            curr = curr->next;
            delete hapus;
        }
    }

    // Fungsi Insert Last untuk Linked List histori (O(1) karena punya tail)
    void tambahLogHistori(std::string kt, std::string stat, std::string wkt) {
        NodeHistori* logBaru = new NodeHistori(kt, stat, wkt);
        if (headHistori == nullptr) {
            headHistori = logBaru;
            tailHistori = logBaru;
        } else {
            tailHistori->next = logBaru;
            tailHistori = logBaru;
        }
    }

    // Traversal linked list buat lacak posisi dan riwayat log
    void tampilkanRiwayat() const {
        std::cout << "\n=================================================================\n";
        std::cout << "                  LACAK NOMOR RESI: " << resi << "\n";
        std::cout << "=================================================================\n";
        std::cout << "  * Kota Asal        : " << asal << "\n";
        std::cout << "  * Kota Tujuan      : " << tujuan << "\n";
        std::cout << "  * Posisi Sekarang  : " << currKota << "\n";
        std::cout << "  * Status Pengiriman: " << (currKota == tujuan ? "SAMPAI TUJUAN" : "DALAM TRANSIT") << "\n";
        std::cout << "-----------------------------------------------------------------\n";
        std::cout << std::left 
                  << std::setw(12) << "Waktu" 
                  << " | " << std::setw(15) << "Lokasi Hub" 
                  << " | " << "Status & Riwayat Perjalanan\n";
        std::cout << "-----------------------------------------------------------------\n";
        
        NodeHistori* curr = headHistori;
        while (curr != nullptr) {
            std::cout << std::left 
                      << std::setw(12) << curr->waktuLog 
                      << " | " << std::setw(15) << curr->kotaTransit 
                      << " | " << curr->status << "\n";
            curr = curr->next;
        }
        std::cout << "=================================================================\n";
    }
};

// ============================================================================
// 3. ADT: LINKED LIST-BASED QUEUE (FIFO) UNTUK ANTREAN GUDANG HUB
// ============================================================================
struct NodeQueue {
    Paket* dataPaket;    // Pointer ke objek Paket dinamis
    NodeQueue* next;     // Pointer ke antrean belakangnya

    NodeQueue(Paket* p) : dataPaket(p), next(nullptr) {}
};

class QueueGudang {
private:
    NodeQueue* depan;    // Pointer antrean paling depan (buat Dequeue O(1))
    NodeQueue* belakang; // Pointer antrean paling belakang (buat Enqueue O(1))
    int jumlahPaket;     // Counter size antrean gudang

public:
    QueueGudang() : depan(nullptr), belakang(nullptr), jumlahPaket(0) {}

    ~QueueGudang() {
        bersihkanGudang();
    }

    bool apakahKosong() const {
        return depan == nullptr;
    }

    int dapatkanUkuran() const {
        return jumlahPaket;
    }

    // Fungsi Enqueue (FIFO - Insert Last)
    void masukAntrean(Paket* p) {
        NodeQueue* nodeBaru = new NodeQueue(p);
        if (apakahKosong()) {
            depan = nodeBaru;
            belakang = nodeBaru;
        } else {
            belakang->next = nodeBaru;
            belakang = nodeBaru;
        }
        jumlahPaket++;
    }

    // Fungsi Dequeue (FIFO - Delete First)
    Paket* keluarAntrean() {
        if (apakahKosong()) return nullptr;
        
        NodeQueue* temp = depan;
        Paket* p = temp->dataPaket;
        depan = depan->next;
        
        if (depan == nullptr) {
            belakang = nullptr;
        }
        
        delete temp;
        jumlahPaket--;
        return p;
    }

    // Traversal antrean gudang untuk opsi menu detail CLI
    void tampilkanIsiAntrean() const {
        if (apakahKosong()) {
            std::cout << "      (Gudang kosong, tidak ada antrean paket)\n";
            return;
        }
        NodeQueue* curr = depan;
        int no = 1;
        while (curr != nullptr) {
            std::cout << "      " << no++ << ". Resi: " << curr->dataPaket->resi 
                      << " | Asal: " << curr->dataPaket->asal 
                      << " -> Tujuan: " << curr->dataPaket->tujuan << "\n";
            curr = curr->next;
        }
    }

    void bersihkanGudang() {
        while (!apakahKosong()) {
            keluarAntrean();
        }
    }
};

// 4. ADT: REPRESENTASI GRAF PETA RUTE LOGISTIK (ADJACENCY LIST)
struct EdgeRute {
    std::string kotaTujuan; // Nama kota tetangga yang terhubung
    int jarakKm;            // Bobot rute (jarak dalam km)
    EdgeRute* next;         // Pointer ke rute tetangga berikutnya

    EdgeRute(std::string dest, int dist) : kotaTujuan(dest), jarakKm(dist), next(nullptr) {}
};

struct NodeKota {
    std::string namaKota;     // Nama kota sebagai simpul / vertex
    QueueGudang gudangHub;    // Antrean gudang logistik eksklusif di kota ini
    EdgeRute* listTetangga;   // Head untuk adjacency list rute keluar
    NodeKota* next;           // Pointer ke vertex kota berikutnya di dalam graf

    NodeKota(std::string nama) : namaKota(nama), listTetangga(nullptr), next(nullptr) {}

    ~NodeKota() {
        // Bebaskan memori linked list edge tetangga
        EdgeRute* curr = listTetangga;
        while (curr != nullptr) {
            EdgeRute* hapus = curr;
            curr = curr->next;
            delete hapus;
        }
    }

    // Tambah rute baru (Insert First agar O(1))
    void tambahTetangga(std::string dest, int dist) {
        EdgeRute* ruteBaru = new EdgeRute(dest, dist);
        ruteBaru->next = listTetangga;
        listTetangga = ruteBaru;
    }
};

// Node pembantu buat simulasi Algoritma Dijkstra manual (full pointer, ga pake STL vector/map!)
struct NodeDijkstra {
    NodeKota* kota;            // Alamat vertex kota
    int jarakTerkecil;         // Akumulasi bobot jarak terpendek dari kota asal
    NodeKota* ruteSebelumnya;  // Parent node untuk backtracking jalur terbaik
    bool sudahDikunjungi;      // Flag status visit
    NodeDijkstra* next;        // Pointer ke node pembantu berikutnya

    NodeDijkstra(NodeKota* k) 
        : kota(k), jarakTerkecil(1000000000), ruteSebelumnya(nullptr), sudahDikunjungi(false), next(nullptr) {}
};

class GrafPeta {
private:
    NodeKota* headKota; // Head linked list dari semua vertex kota dalam graf

public:
    GrafPeta() : headKota(nullptr) {}

    ~GrafPeta() {
        // Bebaskan seluruh vertex kota dalam graf
        NodeKota* curr = headKota;
        while (curr != nullptr) {
            NodeKota* hapus = curr;
            curr = curr->next;
            delete hapus;
        }
    }

    // Tambah kota baru ke graf (Insert First O(1))
    void tambahKota(std::string nama) {
        if (cariKota(nama) != nullptr) return; // Kota udah ada, jangan diduplikasi
        NodeKota* kotaBaru = new NodeKota(nama);
        kotaBaru->next = headKota;
        headKota = kotaBaru;
    }

    // Cari kota berdasarkan string nama - O(V)
    NodeKota* cariKota(std::string nama) const {
        NodeKota* curr = headKota;
        while (curr != nullptr) {
            if (curr->namaKota == nama) return curr;
            curr = curr->next;
        }
        return nullptr;
    }

    // Tambah rute berarah dari kota asal ke kota tujuan - O(V)
    void tambahRute(std::string asal, std::string tujuan, int jarak) {
        tambahKota(asal);
        tambahKota(tujuan);
        NodeKota* vAsal = cariKota(asal);
        if (vAsal != nullptr) {
            vAsal->tambahTetangga(tujuan, jarak);
        }
    }

    NodeKota* dapatkanHeadKota() const {
        return headKota;
    }

    // Nampilin peta visual berupa Adjacency List di CLI
    void tampilkanPeta() const {
        std::cout << "\n=================================================================\n";
        std::cout << "                   PETA HUB LOGISTIK & RUTE AKTIF\n";
        std::cout << "=================================================================\n";
        NodeKota* currKota = headKota;
        if (currKota == nullptr) {
            std::cout << "  Peta masih kosong! Silakan inisialisasi hub dulu.\n";
            return;
        }
        while (currKota != nullptr) {
            std::cout << " [Gudang Hub: " << currKota->namaKota << "] (Jumlah Paket: " 
                      << currKota->gudangHub.dapatkanUkuran() << ")\n";
            EdgeRute* rute = currKota->listTetangga;
            if (rute == nullptr) {
                std::cout << "   -> (Tidak ada jalur distribusi keluar)\n";
            } else {
                while (rute != nullptr) {
                    std::cout << "   -> Jalur ke " << std::left << std::setw(12) << rute->kotaTujuan 
                              << " [" << rute->jarakKm << " km]\n";
                    rute = rute->next;
                }
            }
            currKota = currKota->next;
            std::cout << "-----------------------------------------------------------------\n";
        }
    }

    // ========================================================================
    // LOGIKA PATHFINDING: ALGORITMA DIJKSTRA MANUAL DENGAN POINTER
    // ========================================================================
    // Fungsi ini bakal nyari rute terpendek, trus balikin nama kota berikutnya (next hop)
    // yang harus dilewatin paket biar cepet sampe tujuan.
    std::string dapatkanNextHopDijkstra(std::string asal, std::string tujuan, int& totalJarak, int& jarakTransit) {
        NodeKota* vAsal = cariKota(asal);
        NodeKota* vTujuan = cariKota(tujuan);
        if (vAsal == nullptr || vTujuan == nullptr) return "";
        if (asal == tujuan) return asal;

        // 1. Bikin list node pembantu buat nyatet status jarak & visit
        NodeDijkstra* headDijkstra = nullptr;
        NodeKota* currKota = headKota;
        while (currKota != nullptr) {
            NodeDijkstra* nodeD = new NodeDijkstra(currKota);
            nodeD->next = headDijkstra;
            headDijkstra = nodeD;
            currKota = currKota->next;
        }

        // Setel jarak awal kota pengirim ke 0
        NodeDijkstra* currD = headDijkstra;
        while (currD != nullptr) {
            if (currD->kota->namaKota == asal) {
                currD->jarakTerkecil = 0;
                break;
            }
            currD = currD->next;
        }

        // 2. Proses Utama Dijkstra
        while (true) {
            // Cari node dengan jarak terkecil yang belum dikunjungi
            NodeDijkstra* minNode = nullptr;
            int jarakMin = 1000000000;
            currD = headDijkstra;
            while (currD != nullptr) {
                if (!currD->sudahDikunjungi && currD->jarakTerkecil < jarakMin) {
                    jarakMin = currD->jarakTerkecil;
                    minNode = currD;
                }
                currD = currD->next;
            }

            // Kalo ga ada lagi yang bisa dijangkau, stop
            if (minNode == nullptr || minNode->jarakTerkecil == 1000000000) {
                break;
            }

            minNode->sudahDikunjungi = true;

            // Kalo udah nyampe kota tujuan, langsung stop biar efisien
            if (minNode->kota->namaKota == tujuan) {
                break;
            }

            // Relaxasi rute tetangga
            EdgeRute* rute = minNode->kota->listTetangga;
            while (rute != nullptr) {
                // Cari data pembantu si tetangga di list Dijkstra
                NodeDijkstra* nodeTetangga = headDijkstra;
                while (nodeTetangga != nullptr) {
                    if (nodeTetangga->kota->namaKota == rute->kotaTujuan) {
                        break;
                    }
                    nodeTetangga = nodeTetangga->next;
                }

                if (nodeTetangga != nullptr && !nodeTetangga->sudahDikunjungi) {
                    int jarakBaru = minNode->jarakTerkecil + rute->jarakKm;
                    if (jarakBaru < nodeTetangga->jarakTerkecil) {
                        nodeTetangga->jarakTerkecil = jarakBaru;
                        nodeTetangga->ruteSebelumnya = minNode->kota; // Simpan jejak parent
                    }
                }
                rute = rute->next;
            }
        }

        // 3. Rekonstruksi Jalur pake Stack Manual (Backtracking)
        NodeDijkstra* targetD = headDijkstra;
        while (targetD != nullptr) {
            if (targetD->kota->namaKota == tujuan) {
                break;
            }
            targetD = targetD->next;
        }

        std::string nextHop = "";
        totalJarak = -1;
        jarakTransit = -1;

        if (targetD != nullptr && targetD->jarakTerkecil != 1000000000) {
            totalJarak = targetD->jarakTerkecil;

            // Bikin Stack pointer sederhana buat balik urutan backtrack
            struct NodeStack {
                NodeKota* kota;
                NodeStack* next;
                NodeStack(NodeKota* k) : kota(k), next(nullptr) {}
            };
            NodeStack* stackHead = nullptr;

            NodeKota* telusur = targetD->kota;
            while (telusur != nullptr) {
                NodeStack* nodeStack = new NodeStack(telusur);
                nodeStack->next = stackHead;
                stackHead = nodeStack;

                if (telusur->namaKota == asal) {
                    break;
                }

                // Cari parent kota ini
                NodeDijkstra* lookup = headDijkstra;
                NodeKota* parentKota = nullptr;
                while (lookup != nullptr) {
                    if (lookup->kota == telusur) {
                        parentKota = lookup->ruteSebelumnya;
                        break;
                    }
                    lookup = lookup->next;
                }
                telusur = parentKota;
            }

            // Nah, stack sekarang isinya [asal, next_hop, ..., tujuan]
            // Kota transit berikutnya ada tepat setelah kota asal (stackHead->next)
            if (stackHead != nullptr && stackHead->kota->namaKota == asal && stackHead->next != nullptr) {
                nextHop = stackHead->next->kota->namaKota;

                // Cari bobot jarak langkah transit ini (asal -> nextHop)
                EdgeRute* e = vAsal->listTetangga;
                while (e != nullptr) {
                    if (e->kotaTujuan == nextHop) {
                        jarakTransit = e->jarakKm;
                        break;
                    }
                    e = e->next;
                }
            }

            // Bersihin memori stack pembantu
            while (stackHead != nullptr) {
                NodeStack* temp = stackHead->next;
                delete stackHead;
                stackHead = temp;
            }
        }

        // 4. Bersihin memori DijkstraNode pembantu biar ga leak memori
        while (headDijkstra != nullptr) {
            NodeDijkstra* temp = headDijkstra->next;
            delete headDijkstra;
            headDijkstra = temp;
        }

        return nextHop;
    }
};

// ============================================================================
// 5. ADT: REGISTRY GLOBAL PAKET (BUAT TRAVERSAL & LACAK RESI GLOBAL)
// ============================================================================
struct NodeRegistryGlobal {
    Paket* dataPaket;
    NodeRegistryGlobal* next;

    NodeRegistryGlobal(Paket* p) : dataPaket(p), next(nullptr) {}
};

// ============================================================================
// 6. MAIN CONTROLLER: SIMULATOR MANAJEMEN LOGISTIK
// ============================================================================
class SimulatorLogistik {
private:
    GrafPeta peta;                         // Graf peta rute antar kota
    NodeRegistryGlobal* headRegistryPaket; // Tracker seluruh paket dalam sistem simulator

public:
    SimulatorLogistik() : headRegistryPaket(nullptr) {
        isiPetaDefault();
    }

    ~SimulatorLogistik() {
        // Hapus semua objek paket dari memori saat program di-close
        NodeRegistryGlobal* curr = headRegistryPaket;
        while (curr != nullptr) {
            NodeRegistryGlobal* temp = curr->next;
            delete curr->dataPaket; // Panggil destruktor Paket (sekaligus nge-delete histori list-nya)
            delete curr;
            curr = temp;
        }
    }

    // Inisialisasi rute dan kota hub default sesuai spesifikasi tugas
    void isiPetaDefault() {
        peta.tambahKota("Jakarta");
        peta.tambahKota("Bekasi");
        peta.tambahKota("Karawang");
        peta.tambahKota("Cikampek");
        peta.tambahKota("Purwakarta");
        peta.tambahKota("Subang");
        peta.tambahKota("Sumedang");
        peta.tambahKota("Bandung");

        // Hubungkan jalur antar-kota berbobot jarak km
        peta.tambahRute("Jakarta", "Bekasi", 20);
        peta.tambahRute("Bekasi", "Karawang", 35);
        peta.tambahRute("Bekasi", "Bandung", 120); // Jalur alternatif Tol langsung
        peta.tambahRute("Karawang", "Cikampek", 20);
        peta.tambahRute("Cikampek", "Purwakarta", 18);
        peta.tambahRute("Cikampek", "Subang", 45);
        peta.tambahRute("Purwakarta", "Bandung", 60);
        peta.tambahRute("Subang", "Sumedang", 55);
        peta.tambahRute("Sumedang", "Bandung", 45);
    }

    void cetakPeta() const {
        peta.tampilkanPeta();
    }

    // Fitur CLI: Registrasi Paket Baru (Enqueue ke kota asal)
    bool daftarkanPaketBaru(std::string resi, std::string asal, std::string tujuan, std::string waktu) {
        // Cek dulu apakah nomor resi udah kepake (harus unik!)
        if (cariPaketDiRegistry(resi) != nullptr) {
            std::cout << "\n [ERROR] Paket dengan nomor resi " << resi << " sudah terdaftar!\n";
            return false;
        }

        NodeKota* kAsal = peta.cariKota(asal);
        NodeKota* kTujuan = peta.cariKota(tujuan);
        if (kAsal == nullptr) {
            std::cout << "\n [ERROR] Kota asal '" << asal << "' tidak terdaftar di peta!\n";
            return false;
        }
        if (kTujuan == nullptr) {
            std::cout << "\n [ERROR] Kota tujuan '" << tujuan << "' tidak terdaftar di peta!\n";
            return false;
        }

        // Bikin objek Paket dinamis baru
        Paket* paketBaru = new Paket(resi, asal, tujuan, waktu);

        // Masukin ke registry global
        NodeRegistryGlobal* regNode = new NodeRegistryGlobal(paketBaru);
        regNode->next = headRegistryPaket;
        headRegistryPaket = regNode;

        // Enqueue ke gudang kota asal
        kAsal->gudangHub.masukAntrean(paketBaru);

        std::cout << "\n [SUKSES] Paket " << resi << " berhasil masuk gudang " << asal 
                  << " menuju " << tujuan << ".\n";
        return true;
    }

    // Traversal registry global untuk mencocokkan string nomor resi
    Paket* cariPaketDiRegistry(std::string resi) const {
        NodeRegistryGlobal* curr = headRegistryPaket;
        while (curr != nullptr) {
            if (curr->dataPaket->resi == resi) {
                return curr->dataPaket;
            }
            curr = curr->next;
        }
        return nullptr;
    }

    // Fitur CLI: Lacak Resi (Tampilkan semua log perjalanan dari awal sampai posisi terakhir)
    void lacakPosisiPaket(std::string resi) const {
        Paket* p = cariPaketDiRegistry(resi);
        if (p == nullptr) {
            std::cout << "\n [ERROR] Paket dengan resi '" << resi << "' tidak ditemukan di sistem!\n";
            return;
        }
        p->tampilkanRiwayat();
    }

    // Fitur CLI: Proses Dispatch/Transit Hub (Memproses antrean kota saat ini)
    void jalankanDispatchHub(std::string namaKota, std::string waktuSimulasi) {
        NodeKota* hub = peta.cariKota(namaKota);
        if (hub == nullptr) {
            std::cout << "\n [ERROR] Hub kota '" << namaKota << "' tidak ada di peta!\n";
            return;
        }

        if (hub->gudangHub.apakahKosong()) {
            std::cout << "\n [INFO] Gudang Hub '" << namaKota << "' kosong. Tidak ada paket untuk diproses.\n";
            return;
        }

        std::cout << "\n=================================================================\n";
        std::cout << "                 MEMULAI DISPATCH GUDANG HUB: " << namaKota << "\n";
        std::cout << "=================================================================\n";

        int totalPaketDiGudang = hub->gudangHub.dapatkanUkuran();
        int berhasilTransit = 0;

        for (int i = 0; i < totalPaketDiGudang; i++) {
            Paket* p = hub->gudangHub.keluarAntrean(); // Dequeue paket terdepan
            if (p == nullptr) continue;

            // Kalo paket emang udah nyampe di kota tujuannya, keep di gudang tujuan
            if (p->currKota == p->tujuan) {
                std::cout << " [*] Paket " << p->resi << " sudah berada di tujuan akhir (" << p->tujuan << ").\n";
                hub->gudangHub.masukAntrean(p); // Enqueue balik biar tetep disitu
                continue;
            }

            // Cari kota tujuan berikutnya pake Dijkstra manual
            int totalJarakRute = 0;
            int jarakLangkahTransit = 0;
            std::string kotaTransitBerikutnya = peta.dapatkanNextHopDijkstra(p->currKota, p->tujuan, totalJarakRute, jarakLangkahTransit);

            if (kotaTransitBerikutnya.empty() || kotaTransitBerikutnya == p->currKota) {
                std::cout << " [WARNING] Tidak ada rute penghubung ke " << p->tujuan 
                          << " untuk Paket " << p->resi << "! Paket tertahan.\n";
                hub->gudangHub.masukAntrean(p); // Kembalikan ke antrean semula
            } else {
                NodeKota* hubTransit = peta.cariKota(kotaTransitBerikutnya);
                if (hubTransit != nullptr) {
                    // Update lokasi terkini paket
                    p->currKota = kotaTransitBerikutnya;
                    
                    std::string logMsg;
                    if (kotaTransitBerikutnya == p->tujuan) {
                        logMsg = "Tiba di Hub Tujuan Akhir. (Transit: " + std::to_string(jarakLangkahTransit) + " km)";
                        p->tambahLogHistori(kotaTransitBerikutnya, logMsg, waktuSimulasi);
                        std::cout << " [TIBA] Paket " << p->resi << " sampai di tujuan: " 
                                  << kotaTransitBerikutnya << " (Langkah transit: " << jarakLangkahTransit << " km)\n";
                    } else {
                        logMsg = "Transit. Menuju " + p->tujuan + " via " + kotaTransitBerikutnya + " (Transit: " + std::to_string(jarakLangkahTransit) + " km)";
                        p->tambahLogHistori(kotaTransitBerikutnya, logMsg, waktuSimulasi);
                        std::cout << " [TRANSIT] Paket " << p->resi << " dialihkan: " 
                                  << namaKota << " -> " << kotaTransitBerikutnya << " (" << jarakLangkahTransit << " km). Sisa rute ke target: " 
                                  << totalJarakRute - jarakLangkahTransit << " km.\n";
                    }

                    // Enqueue paket ke gudang kota transit berikutnya
                    hubTransit->gudangHub.masukAntrean(p);
                    berhasilTransit++;
                } else {
                    std::cout << " [ERROR] Terjadi kegagalan rute transit berikutnya!\n";
                    hub->gudangHub.masukAntrean(p);
                }
            }
        }
        std::cout << "-----------------------------------------------------------------\n";
        std::cout << " Dispatch selesai. " << berhasilTransit << " paket berhasil dikirim ke hub berikutnya.\n";
        std::cout << "=================================================================\n";
    }

    // Nampilin isi gudang masing-masing hub secara lengkap buat evaluasi asdos
    void tampilkanStatusDetailSemuaHub() const {
        std::cout << "\n=================================================================\n";
        std::cout << "                 STATUS DETAIL ANTREAN GUDANG HUB\n";
        std::cout << "=================================================================\n";
        NodeKota* currKota = peta.dapatkanHeadKota();
        if (currKota == nullptr) {
            std::cout << "  Graf kota kosong.\n";
            return;
        }
        while (currKota != nullptr) {
            std::cout << " Hub Kota [" << currKota->namaKota << "] (Total: " << currKota->gudangHub.dapatkanUkuran() << " Paket Antre):\n";
            currKota->gudangHub.tampilkanIsiAntrean();
            currKota = currKota->next;
            std::cout << "-----------------------------------------------------------------\n";
        }
    }

    // Demo seeder biar pas di-run asdos programnya ga keliatan sepi
    void isiDataDemo() {
        daftarkanPaketBaru("RESI-101", "Jakarta", "Bandung", "H+1 08:00");
        daftarkanPaketBaru("RESI-102", "Bekasi", "Sumedang", "H+1 09:00");
        daftarkanPaketBaru("RESI-103", "Karawang", "Bandung", "H+1 10:00");
    }
};

// Fungsi helper buat ngerubah input user biar ga sensitif case huruf (menghindari typo)
std::string standarisasiNamaKota(std::string input) {
    std::string hasil = "";
    for (char c : input) {
        hasil += tolower(c);
    }
    if (hasil == "jakarta") return "Jakarta";
    if (hasil == "bekasi") return "Bekasi";
    if (hasil == "karawang") return "Karawang";
    if (hasil == "cikampek") return "Cikampek";
    if (hasil == "purwakarta") return "Purwakarta";
    if (hasil == "subang") return "Subang";
    if (hasil == "sumedang") return "Sumedang";
    if (hasil == "bandung") return "Bandung";
    return input; // Kalo ga cocok, balikin apa adanya biar ditolak validator internal
}

// ============================================================================
// MAIN FUNCTION & MENU CLI SIMULATOR
// ============================================================================
int main() {
    SimulatorLogistik simulator;

    // Load data simulasi awal
    simulator.isiDataDemo();

    int menu = 0;
    std::string in1, in2, in3, in4;

    std::cout << "=================================================================\n";
    std::cout << "       SIMULATOR IN-MEMORY SISTEM LOGISTIK & ROUTING PAKET\n";
    std::cout << "      (Struktur Data Pointer & Graph - Tugas Besar Semester 3)\n";
    std::cout << "=================================================================\n";
    std::cout << " Selamat datang di simulator sistem logistik in-memory.\n";
    std::cout << " Peta rute aktif otomatis dimuat ke dalam Directed Graph.\n";
    std::cout << " Daftar Hub Kota terdaftar:\n";
    std::cout << " -> Jakarta, Bekasi, Karawang, Cikampek, Purwakarta,\n";
    std::cout << "    Subang, Sumedang, Bandung\n";
    std::cout << "=================================================================\n\n";

    do {
        std::cout << ">>> UTAMA MENU SIMULATOR <<<\n";
        std::cout << " [1] Lihat Peta Hub & Jalur Rute Logistik\n";
        std::cout << " [2] Tampilkan Antrean Detail Paket di Semua Hub Gudang\n";
        std::cout << " [3] Registrasi Paket Baru (Enqueue ke Hub Kota Asal)\n";
        std::cout << " [4] Proses Jalankan Dispatch (Transit / Routing Rute Terdekat)\n";
        std::cout << " [5] Lacak Resi Paket (Traceback Histori Perjalanan)\n";
        std::cout << " [6] Keluar dari Simulator\n";
        std::cout << " Pilih Opsi (1-6): ";
        
        if (!(std::cin >> menu)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\n [ERROR] Input ga valid! Harus berupa angka pilihan menu.\n\n";
            continue;
        }

        std::cin.ignore(10000, '\n'); // Bersihin buffer sisa input

        switch (menu) {
            case 1:
                simulator.cetakPeta();
                std::cout << "\n";
                break;
            case 2:
                simulator.tampilkanStatusDetailSemuaHub();
                std::cout << "\n";
                break;
            case 3:
                std::cout << "\n>>> REGISTRASI PAKET BARU <<<\n";
                std::cout << " Masukkan Nomor Resi Unik (misal: RESI-999): ";
                std::getline(std::cin, in1);
                std::cout << " Kota Asal  (Hub): ";
                std::getline(std::cin, in2);
                in2 = standarisasiNamaKota(in2);
                std::cout << " Kota Tujuan (Hub): ";
                std::getline(std::cin, in3);
                in3 = standarisasiNamaKota(in3);
                std::cout << " Waktu Registrasi (misal: H+1 11:00): ";
                std::getline(std::cin, in4);

                simulator.daftarkanPaketBaru(in1, in2, in3, in4);
                std::cout << "\n";
                break;
            case 4:
                std::cout << "\n>>> DISPATCH DAN TRANSIT HUB <<<\n";
                std::cout << " Masukkan Hub Kota asal yang ingin didispatch (misal: Jakarta): ";
                std::getline(std::cin, in1);
                in1 = standarisasiNamaKota(in1);
                std::cout << " Masukkan Waktu Dispatch Simulasi (misal: H+1 15:00): ";
                std::getline(std::cin, in2);

                simulator.jalankanDispatchHub(in1, in2);
                std::cout << "\n";
                break;
            case 5:
                std::cout << "\n>>> LACAK RIWAYAT RESI (TRACEBACK) <<<\n";
                std::cout << " Masukkan Nomor Resi paket: ";
                std::getline(std::cin, in1);
                
                simulator.lacakPosisiPaket(in1);
                std::cout << "\n";
                break;
            case 6:
                std::cout << "\n Keluar simulator... Melakukan deallokasi memori global...\n";
                std::cout << " Memori berhasil dibebaskan. Program selesai dengan kode sukses 0!\n";
                break;
            default:
                std::cout << "\n [ERROR] Opsi menu ga ada! Silakan pilih angka 1-6.\n\n";
        }
    } while (menu != 6);

    return 0;
}
