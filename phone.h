#ifndef __PHONE_H__
#define __PHONE_H__

#define ATAG_BASE_ADDR (0x90000100)
#define CMDLINE "noinitrd console=/dev/null rw mem=58M@0x90000000 init=/linuxrc ip=off root=/dev/mtdblock/root brdrev=R3B motobldlabel=R6713_G_71.02.06R mtdparts=nand0:1152k@896k(pds),128k@2176k(logo_cli),128k(panel_script)ro,128k(device_tree)ro,128k(setup),256k(logo),128k(secure),1664k(kern)ro,384k(initramfs)ro,3968k(bpsw),256k(ho_data),256k(fota_ua)ro,384k(ua_temp),256k(ua_backup),49m@10368k(root)ro,9m(user),9728k(lang),34m(resrc),6m(fota_up),7m(mass_storage),128k@127744k(kpanic),3200k(rsv)"

#endif // __PHONE_H__
