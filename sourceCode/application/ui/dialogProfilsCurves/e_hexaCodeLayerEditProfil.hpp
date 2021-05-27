#ifndef E_HEXACODELAYEREDITPROFIL_HPP
#define E_HEXACODELAYEREDITPROFIL_HPP

enum e_hexaCodeComponentEditProfil {
   e_hxCCEP_Perp = 0x01,
   e_hxCCEP_Parall = 0x02,
   e_hxCCEP_Perp_Parall = e_hxCCEP_Perp|e_hxCCEP_Parall,
   e_hxCCEP_deltaZ = 0x04,
   e_hxCCEP_Perp_deltaZ = e_hxCCEP_Perp|e_hxCCEP_deltaZ,
   e_hxCCEP_Parall_deltaZ = e_hxCCEP_Parall|e_hxCCEP_deltaZ,
   e_hxCCEP_All = e_hxCCEP_Perp|e_hxCCEP_Parall|e_hxCCEP_deltaZ
};

#endif // E_HEXACODELAYEREDITPROFIL_HPP
