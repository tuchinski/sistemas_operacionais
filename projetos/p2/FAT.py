# Classe que armazena as informações importantes da imagem FAT32
class fat:
    def __init__(self, **kwargs) -> None:
        prop_defaults = {
            # BPB INFOS
            "jmp_boot": b'-1',
            "oem_name": '-1',
            'bytes_per_sec': -1,
            'sector_per_cluster': -1,
            'reserved_sectors': -1,
            'num_fats': -1,
            'media': -1,
            'sec_per_trk': -1,
            'num_heads': -1,
            'tot_sec_32': -1,
            'fat_Sz_32': -1,
            'fat_size_bytes': -1,
            'ext_flags': '',
            'fs_ver': '',
            'root_clus': -1,
            'fs_info': -1,
            'bk_boot_sec': -1,
            'reserved_bytes': '-1',
            'fil_sys_type': '-1',

            # FSInfo Sector infos
            'start_FSInfo': -1, 
            'fsi_lead_sig': -1, 
            'fsi_reserved1': -1, 
            'fsi_struc_sig': -1, 
            'fsi_free_count': -1, 
            'fsi_nxt_free': -1, 
            'fsi_reserved2': -1, 
            'fsi_trail_sig': -1, 

            # outros dados
            'start_fat1': 0,
            'root_dir_sectors': -1,
            'first_data_sector': -1,
            'data_sec': -1,
            'count_of_clusters': -1,
            'nome_diretorio_atual': '/',
            'cluster_inicial_diretorio_atual': 2
        }
        for(prop,default) in prop_defaults.items():
            setattr(self, prop, kwargs.get(prop, default))
    

    def __repr__(self) -> str:
        retorno = 'FAT'
        for(key,value) in vars(self).items():
            retorno = retorno + f'\n{key}: {value}'

        return retorno