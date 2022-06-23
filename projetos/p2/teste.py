


def main():
    with open('myimagefat32.img', 'rb') as teste:
        a = teste.read()


        # BS_jmpBoot
        jmpBoot = a[0:3]
        # instrução de jump para o código de boot
        # aceitos somente o primeiro ou o segundo
        # jmpBoot[0] = 0xeb | 0xe9
        # jmpBoot[1] = 0x?? | 0x??
        # jmpBoot[2] = 0x90 | 0x??
        print(f"jmpBoot: {hex(jmpBoot[0])} {hex(jmpBoot[1])} {hex(jmpBoot[2])}")
        
        # somente um campo com um nome
        # BS_OEMName
        oem_name = a[3:11].decode()
        print(f"OEM Name: {oem_name}")

        # qtde de bytes por setor
        # precisa ter os valores 512, 1024, 2048 or 4096
        # BPB_BytsPerSec
        # bytes_per_sec = a[11:13]
        bytes_per_sec = int.from_bytes(a[11:13],'little')
        print(f"bytes per sector: {bytes_per_sec}")

        # setores por unidades alocadas
        # tem que ser potência de 2 e maior que 0
        # BPB_SecPerClus
        sector_per_cluster = a[13]
        print(f"Sector per clusters: {sector_per_cluster}")

        # numero de setores reservados
        # BPB_RsvdSecCnt
        # reserved_sectors = a[14:16]
        reserved_sectors = int.from_bytes(a[14:16],'little')
        print(f"Reserved Sectors: {reserved_sectors}")

        # numero de estruturas FAT no disco
        # precisa ser 2
        # BPB_NumFATs
        num_fats = a[16]
        print(f"Number of FAT data structures on the volume: {num_fats}")

        # Valor padrao 0xf8
        # BPB_Media
        media = a[21]
        print(f"Media: {hex(media)}")

        # Setores por trilha
        # BPB_SecPerTrk
        sec_per_trk = a[24:26]
        print(f"Sectors per track: {int.from_bytes(sec_per_trk,'little')}")

        # Number of heads for interrupt 0x13
        # BPB_NumHeads
        num_heads = a[26:28]
        print(f"Number of heads: {int.from_bytes(num_heads,'little')}")

        # total de setores de 32 bits
        # BPB_TotSec32
        # tot_sec_32 = a[32:36]
        tot_sec_32 = int.from_bytes(a[32:36],'little')
        print(f"total sectors: {tot_sec_32}")

        # Setores por FAT
        # BPB_FATSz32
        # fat_Sz_32 = a[36:40]
        fat_Sz_32 = int.from_bytes(a[36:40],'little')
        # print(f"FAT32 32-bit count of sectors occupied by ONE FAT: {fat_Sz_32}")
        print(f"Sectors occupied by FAT: {fat_Sz_32}")

        # tamanho da FAT em bytes
        fat_size_bytes = fat_Sz_32 * bytes_per_sec
        print(f"FAT size: {fat_size_bytes} bytes")

        # flags externas
        # BPB_ExtFlags
        ext_flags = a[40:42]
        print(f"external flags: {ext_flags}")

        # número da versão
        # doc da microsoft é 0:0
        # BPB_FSVer
        fs_ver = a[42:44]
        print(f"version number: {fs_ver}")

        # Cluster raiz
        # Geralmente valor é 2
        # BPB_RootClus
        root_clus = a[44:48]
        print(f"root cluster: {int.from_bytes(root_clus,'little')}")

        # número do setor do FSINFO
        # BPB_FSInfo
        fs_info = a[48:50]
        print(f"Sector number of FSINFO: {int.from_bytes(fs_info,'little')}")

        # indicates the sector number in the reserved area of the volume of a copy of the boot record.
        # indica o numero do setor na area reservada do volume da copia do boot record
        # padrao = 6
        # BPB_BkBootSec
        bk_boot_sec = a[50:52]
        print(f"sector number in the reserved area of the volume of a copy of the boot record: {int.from_bytes(bk_boot_sec,'little')}")

        # bytes reservados
        # geralmente é tudo 0
        # BPB_Reserved
        reserved = a[52:64]
        print(f"Reserved: {reserved}")

        # nome do tipo
        # sempre vai ser FAT32
        # BS_FilSysType
        fil_sys_type = a[82:90]
        print(f"FAT type: {fil_sys_type.decode()}")

        # validacoes extras
        print(a[510]) # tem que ser igual a 0x55 ou 85
        print(a[511]) # tem que ser igual a 0xaa ou 170

        # root_dir_sectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;
        # o BPB_RootEntCnt na FAT32 sempre tem o valor 0
        # por isso o root_dir_sector vai ser 0 tbm na FAT32
        root_dir_sectors = int(((0*32) + (bytes_per_sec - 1)) / bytes_per_sec)
        print(f"root_dir_sectors {root_dir_sectors}")

        # FirstDataSector = BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors;
        first_data_sector = (reserved_sectors * bytes_per_sec) + (num_fats * fat_size_bytes) + root_dir_sectors
        print(f"first_data_sector {hex(first_data_sector)}")

        # FirstSectorofCluster = ((N – 2) * BPB_SecPerClus) + FirstDataSector;
        # como achar o primeiro setor de um cluster N
        n=3
        first_sector_of_cluster = ((n-2) * sector_per_cluster) + first_data_sector
        first_sector_of_cluster = ((n-2) * sector_per_cluster * bytes_per_sec) + first_data_sector
        print(f"first_sector_of_cluster {n} {hex(first_sector_of_cluster)}")

        # DataSec = TotSec – (BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors);
        data_sec = tot_sec_32 - (reserved_sectors + (num_fats * fat_Sz_32) + root_dir_sectors)
        # print(f"{tot_sec_32} - ({reserved_sectors} + ({num_fats} * {fat_Sz_32}) + {root_dir_sectors})")
        print(f"data_sec {data_sec}")

        # CountofClusters = DataSec / BPB_SecPerClus;

        count_of_clusters = int(data_sec / sector_per_cluster)
        print(f"count_of_clusters {count_of_clusters}")










if __name__ == '__main__':
    main()