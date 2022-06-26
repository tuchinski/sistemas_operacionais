from distutils.util import byte_compile
from FAT import fat
import codecs

main_fat = fat()

def read_BPB(imagem):
        global main_fat
        # BS_jmpBoot
        jmpBoot = imagem[0:3]
        # instrução de jump para o código de boot
        # aceitos somente o primeiro ou o segundo
        # jmpBoot[0] = 0xeb | 0xe9
        # jmpBoot[1] = 0x?? | 0x??
        # jmpBoot[2] = 0x90 | 0x??
        print(f"jmpBoot: {hex(jmpBoot[0])} {hex(jmpBoot[1])} {hex(jmpBoot[2])}")
        main_fat.jmp_boot = jmpBoot
        
        # somente um campo com um nome
        # BS_OEMName
        oem_name = imagem[3:11].decode()
        print(f"OEM Name: {oem_name}")

        # qtde de bytes por setor
        # precisa ter os valores 512, 1024, 2048 or 4096
        # BPB_BytsPerSec
        # bytes_per_sec = imagem[11:13]
        bytes_per_sec = int.from_bytes(imagem[11:13],'little')
        print(f"bytes per sector: {bytes_per_sec}")

        # setores por unidades alocadas
        # tem que ser potência de 2 e maior que 0
        # BPB_SecPerClus
        sector_per_cluster = imagem[13]
        print(f"Sector per clusters: {sector_per_cluster}")

        # numero de setores reservados
        # BPB_RsvdSecCnt
        # reserved_sectors = imagem[14:16]
        reserved_sectors = int.from_bytes(imagem[14:16],'little')
        print(f"Reserved Sectors: {reserved_sectors}")

        # numero de estruturas FAT no disco
        # precisa ser 2
        # BPB_NumFATs
        num_fats = imagem[16]
        print(f"Number of FAT data structures on the volume: {num_fats}")

        # Valor padrao 0xf8
        # BPB_Media
        media = imagem[21]
        print(f"Media: {hex(media)}")

        # Setores por trilha
        # BPB_SecPerTrk
        sec_per_trk = imagem[24:26]
        print(f"Sectors per track: {int.from_bytes(sec_per_trk,'little')}")

        # Number of heads for interrupt 0x13
        # BPB_NumHeads
        num_heads = imagem[26:28]
        print(f"Number of heads: {int.from_bytes(num_heads,'little')}")

        # total de setores de 32 bits
        # BPB_TotSec32
        # tot_sec_32 = imagem[32:36]
        tot_sec_32 = int.from_bytes(imagem[32:36],'little')
        print(f"total sectors: {tot_sec_32}")

        # Setores por FAT
        # BPB_FATSz32
        # fat_Sz_32 = imagem[36:40]
        fat_Sz_32 = int.from_bytes(imagem[36:40],'little')
        # print(f"FAT32 32-bit count of sectors occupied by ONE FAT: {fat_Sz_32}")
        print(f"Sectors occupied by FAT: {fat_Sz_32}")

        # tamanho da FAT em bytes
        fat_size_bytes = fat_Sz_32 * bytes_per_sec
        print(f"FAT size: {fat_size_bytes} bytes")

        # flags externas
        # BPB_ExtFlags
        ext_flags = imagem[40:42]
        print(f"external flags: {ext_flags}")

        # número da versão
        # doc da microsoft é 0:0
        # BPB_FSVer
        fs_ver = imagem[42:44]
        print(f"version number: {fs_ver}")

        # Cluster raiz
        # Geralmente valor é 2
        # BPB_RootClus
        root_clus = int.from_bytes(imagem[44:48],'little')
        print(f"root cluster: {root_clus}")

        # número do setor do FSINFO
        # BPB_FSInfo
        fs_info = imagem[48:50]
        print(f"Sector number of FSINFO: {int.from_bytes(fs_info,'little')}")

        # indicates the sector number in the reserved area of the volume of a copy of the boot record.
        # indica o numero do setor na area reservada do volume da copia do boot record
        # padrao = 6
        # BPB_BkBootSec
        bk_boot_sec = imagem[50:52]
        print(f"sector number in the reserved area of the volume of a copy of the boot record: {int.from_bytes(bk_boot_sec,'little')}")

        # bytes reservados
        # geralmente é tudo 0
        # BPB_Reserved
        reserved = imagem[52:64]
        print(f"Reserved: {reserved}")

        # nome do tipo
        # sempre vai ser FAT32
        # BS_FilSysType
        fil_sys_type = imagem[82:90]
        print(f"FAT type: {fil_sys_type.decode()}")

        # validacoes extras
        print(imagem[510]) # tem que ser igual a 0x55 ou 85
        print(imagem[511]) # tem que ser igual a 0xaa ou 170

        # Atribuindo os valores encontrados para o objeto principal
        main_fat.oem_name = oem_name
        main_fat.bytes_per_sec = bytes_per_sec
        main_fat.sector_per_cluster = sector_per_cluster
        main_fat.reserved_sectors = reserved_sectors
        main_fat.num_fats = num_fats
        main_fat.media = media
        main_fat.sec_per_trk = sec_per_trk
        main_fat.num_heads = num_heads
        main_fat.tot_sec_32 = tot_sec_32
        main_fat.fat_Sz_32 = fat_Sz_32
        main_fat.fat_size_bytes = fat_size_bytes
        main_fat.ext_flags = ext_flags
        main_fat.fs_ver = fs_ver
        main_fat.root_clus = root_clus
        main_fat.fs_info = fs_info
        main_fat.bk_boot_sec = bk_boot_sec
        main_fat.reserved_bytes = reserved
        main_fat.fil_sys_type = fil_sys_type

        # root_dir_sectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;
        # o BPB_RootEntCnt na FAT32 sempre tem o valor 0
        # por isso o root_dir_sector vai ser 0 tbm na FAT32
        main_fat.root_dir_sectors = int(((0*32) + (bytes_per_sec - 1)) / bytes_per_sec)
        print(f"root_dir_sectors {main_fat.root_dir_sectors}")

        # FirstDataSector = BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors;
        main_fat.first_data_sector = (reserved_sectors * bytes_per_sec) + (num_fats * fat_size_bytes) + main_fat.root_dir_sectors
        print(f"first_data_sector {hex(main_fat.first_data_sector)}")

        # DataSec = TotSec – (BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors);
        main_fat.data_sec = tot_sec_32 - (reserved_sectors + (num_fats * fat_Sz_32) + main_fat.root_dir_sectors)
        print(f"{tot_sec_32} - ({reserved_sectors} + ({num_fats} * {fat_Sz_32}) + {main_fat.root_dir_sectors})")
        print(f"data_sec {main_fat.data_sec}")

        # CountofClusters = DataSec / BPB_SecPerClus;
        main_fat.count_of_clusters = int(main_fat.data_sec / sector_per_cluster)
        print(f"count_of_clusters {main_fat.count_of_clusters}")

def read_fsi(imagem):
    global main_fat
    print("\n----------------------------------------------------------------\n")
    print("FAT32 FSInfo Sector Structure and Backup Boot Sector\n")
    
    # dados FAT32 FSInfo Sector Structure and Backup Boot Sector
    # FSI_LeadSig
    # o valor tem que ser 0x41615252
    start_FSInfo = main_fat.bytes_per_sec
    fsi_lead_sig = int.from_bytes(imagem[start_FSInfo:start_FSInfo+4], 'little')
    print(f'fsi_lead_sig {hex(fsi_lead_sig)}')

    # FSI_Reserved1
    # campo reservado
    # tem que ser inicializado com 0
    fsi_reserved1 = int.from_bytes(imagem[start_FSInfo+4 : start_FSInfo + 484], 'little')
    print(f'fsi_reserved1 {fsi_reserved1}')

    # FSI_StrucSig
    # o valor deve ser 0x61417272.
    fsi_struc_sig = int.from_bytes(imagem[start_FSInfo + 484: start_FSInfo + 488], 'little')
    print(f'fsi_struc_sig {hex(fsi_struc_sig)}')

    # FSI_Free_Count
    # ultima contagem de clusters livres
    # for 0xFFFFFFFF, então a contagem é desconhecida e precisa ser calculada
    fsi_free_count =  int.from_bytes(imagem[start_FSInfo + 488: start_FSInfo + 492], 'little')
    print(f'fsi_free_count {fsi_free_count}')

    # FSI_Nxt_Free
    # indica onde o driver pode começar a procurar por clusters livres
    # caso tenha o valor 0xFFFFFFFF vai ter que começar do cluster 2 mesmo
    fsi_nxt_free = int.from_bytes(imagem[start_FSInfo + 492: start_FSInfo + 496], 'little')
    print(f'fsi_nxt_free {fsi_nxt_free}')

    # FSI_Reserved2
    # campo reservado
    # tem que ser inicializado com 0
    fsi_reserved2 = int.from_bytes(imagem[start_FSInfo+496 : start_FSInfo + 508], 'little')
    print(f'fsi_reserved2 {fsi_reserved2}')

    # FSI_TrailSig
    # valor tem que ser 0xAA550000
    # Valida que é de fato um FSInfo sector
    fsi_trail_sig = int.from_bytes(imagem[start_FSInfo + 508: start_FSInfo + 512], 'little')
    print(f'fsi_trail_sig {hex(fsi_trail_sig)}')
    
    # atribuindo os valores para o objeto principal
    main_fat.fsi_lead_sig = fsi_lead_sig
    main_fat.fsi_reserved1 = fsi_reserved1
    main_fat.fsi_struc_sig = fsi_struc_sig
    main_fat.fsi_free_count = fsi_free_count
    main_fat.fsi_nxt_free = fsi_nxt_free
    main_fat.fsi_reserved2 = fsi_reserved2
    main_fat.fsi_trail_sig = fsi_trail_sig

# Retorna os dados 
def return_text_from_cluster(imagem, num_cluster) -> str:
    global main_fat
    if num_cluster < main_fat.root_clus:
        print(f"Os clusters de dados começam em {main_fat.root_clus}")
        return None
    
    first_sector_of_cluster = ((num_cluster-2) * main_fat.sector_per_cluster * main_fat.bytes_per_sec) + main_fat.first_data_sector
    print(f"@@@@@first_sector_of_cluster {num_cluster} {hex(first_sector_of_cluster)}")

    cluster_text = imagem[first_sector_of_cluster: first_sector_of_cluster + main_fat.bytes_per_sec * main_fat.sector_per_cluster]
    return cluster_text.decode(errors='backslashreplace')

def print_menu():
    pass
    print(
        '''
        info: exibe informações do disco e da FAT.

        cluster <num>: exibe o conteúdo do bloco num no formato texto.
        
        pwd: exibe o diretório corrente (caminho absoluto).

        attr <file | dir>: exibe os atributos de um arquivo (file) ou diretório (dir).

        cd <path>: altera o diretório corrente para o definido como path.

        touch <file>: cria o arquivo file com conteúdo vazio.

        mkdir <dir>: cria o diretório dir vazio.

        rm <file>: remove o arquivo file do sistema.

        rmdir <dir>: remove o diretório dir, se estiver vazio.

        cp <source_path> <target_path>: copia um arquivo de origem (source_path) para destino (target_path).

        mv <source_path> <target_path>: move um arquivo de origem (source_path) para destino (target_path).

        rename <file> <newfilename> : renomeia arquivo file para newfilename.

        ls: listar os arquivos e diretórios do diretório corrente.
        
        '''
    )

def main():
    with open('myimagefat32.img', 'rb') as imagem_iso:
        global main_fat
        a = imagem_iso.read()
        print_menu()



        # main_fat = fat()
        read_BPB(a)
        read_fsi(a)
        print('\n\n Printando o obj fat')
        print(main_fat)

        # FirstSectorofCluster = ((N – 2) * BPB_SecPerClus) + FirstDataSector;
        # como achar o primeiro setor de um cluster N
        byte_ret = return_text_from_cluster(a, 2)
        print(byte_ret)
       
       

    print("\n----------------------------------------------------------------\n")

    print("Tentando ler um arquivo da tabela de dados\n\n")

    # print("first_data_sector!!!!!!!!!", hex(main_fat.first_data_sector))
    inicio_dados = main_fat.first_data_sector 
    print(f'dasdasd {hex(inicio_dados)}')
    print(hex(a[inicio_dados + 11]))
    if (a[inicio_dados + 11] == 15): # caso o bit 11 tenha o valor 0x0f, é uma entrada de nome longa e os dados do arquivo vai ficar nos proximos 32 bytes
        print('entrada de arquivos com nome longo, lendo os 32 proximos bytes')
        inicio_dados = inicio_dados + 32
    
    # DIR_Name e DIR_extension
    # o nome curto tem 8 bytes, e a extensao 3 bytes
    dir_name = a[inicio_dados: inicio_dados + 8].decode().strip()
    dir_extension = a[inicio_dados+8: inicio_dados + 11].decode()
    
    print(f'dir_name + dir extension: {dir_name}.{dir_extension}')
    
    # DIR_Attr 
    # READ_ONLY=0x01 
    # HIDDEN=0x02 
    # SYSTEM=0x04 
    # VOLUME_ID=0x08 
    # DIRECTORY=0x10 
    # ARCHIVE=0x20 
    # LFN=READ_ONLY|HIDDEN|SYSTEM|VOLUME_ID
    dir_attr = a[inicio_dados + 11]
    if dir_attr == 1:
        print(f'dir_attr: READ_ONLY')
    elif dir_attr == 2:
        print(f'dir_attr: HIDDEN')
    elif dir_attr == 4:
        print(f'dir_attr: SYSTEM')
    elif dir_attr == 8:
        print(f'dir_attr: VOLUME_ID')
    elif dir_attr == 16:
        print(f'dir_attr: DIRECTORY')
    elif dir_attr == 32:
        print(f'dir_attr: ARCHIVE')

    # DIR_CrtTimeTenth
    dir_crt_time_tenth = a[inicio_dados + 13]
    print(f'dir_crt_time_tenth: {dir_crt_time_tenth}')

    # DIR_CrtTime
    # TODO verificar como faz pra contar as horas
    # dir_crt_time = bin(int.from_bytes(a[inicio_dados + 14: inicio_dados + 16], 'little'))[2:]
    dir_crt_time = a[inicio_dados + 14: inicio_dados + 16]
    # print(f'dir_crt_time: {hex(dir_crt_time)}')

    # DIR_CrtDate
    # TODO verificar como faz pra contar as horas
    dir_crt_date = a[inicio_dados + 16: inicio_dados + 18]
    # print(f'dir_crt_time: {hex(dir_crt_time)}')

    # DIR_FstClusHI
    dir_fst_clus_HI = a[inicio_dados + 20: inicio_dados + 22]
    print(f'dir_fst_clus_HI: {dir_fst_clus_HI}')
    segunda_parte_hi = a[inicio_dados + 21]
    primeira_parte_hi = a[inicio_dados + 20]
    concat_hi  = hex(segunda_parte_hi) + hex(primeira_parte_hi)[2:] # concatenando os hex, a segunda parte vem primeiro pq é little endian
    print(concat_hi)
    

    # DIR_WrtTime
    # TODO verificar como faz pra contar as horas
    dir_wrt_time = a[inicio_dados + 22: inicio_dados + 24]
    # print(f'dir_crt_time: {hex(dir_crt_time)}')

    # DIR_WrtDate
    # TODO verificar como faz pra contar as horas
    dir_wrt_date = a[inicio_dados + 24: inicio_dados + 26]

    # DIR_FstClusLO
    # dir_fst_clus_LO = a[inicio_dados + 26: inicio_dados + 28]
    segunda_parte_lo = a[inicio_dados + 27]
    primeira_parte_lo = a[inicio_dados + 26]
    concat_lo  = hex(segunda_parte_lo) + hex(primeira_parte_lo)[2:] # concatenando os hex, a segunda parte vem primeiro pq é little endian
    print(concat_lo)

    first_cluster_dir = int(concat_hi + concat_lo[2:], 16)
    print(f'first_cluster_dir: {first_cluster_dir} -> {hex(first_cluster_dir)}')   

    # DIR_FileSize
    dir_file_size = int.from_bytes(a[inicio_dados + 28 : inicio_dados + 32], 'little')
    print(f'dir_file_size: {dir_file_size}')





if __name__ == '__main__':
    main()