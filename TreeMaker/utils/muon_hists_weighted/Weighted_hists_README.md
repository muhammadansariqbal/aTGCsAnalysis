# Calculating weighted Muon SF histograms

These scripts are designed to create weighted muon scalefactors to be applied to MC.
Due to the HIP issue, there are 2 sets of SF: one for runs B to F, another for runs G & H.
Although there is no recommended way to combine them for use in MC, we choose to create a lumi-weighted average.
These scripts create these new histograms.
There is one script/dir for each contribution, except for tracking SF, for which there is already a combined version available.

These scripts were originally by Anna Benecke.

## Calculating integrated luminosity

Use the brilcalc tool, see instructions here: https://cms-service-lumi.web.cern.ch/cms-service-lumi/brilwsdoc.html

Integrated lumi for runs B to F:

```
brilcalc lumi --normtag /afs/cern.ch/user/l/lumipro/public/Normtags/normtag_PHYSICS.json -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt --begin 271036 --end 278808

#Summary:
+-------+------+--------+--------+-------------------+------------------+
| nfill | nrun | nls | ncms | totdelivered(/ub) | totrecorded(/ub) |
+-------+------+--------+--------+-------------------+------------------+
| 80 | 237 | 142129 | 142111 | 20611578878.393 | 19695456271.236 |
+-------+------+--------+--------+-------------------+------------------+
```

Integrated luminosty for runs G to H:

```
brilcalc lumi --normtag /afs/cern.ch/user/l/lumipro/public/Normtags/normtag_PHYSICS.json -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt --begin 278820 --end 284044

#Summary:
+-------+------+-------+-------+-------------------+------------------+
| nfill | nrun | nls | ncms | totdelivered(/ub) | totrecorded(/ub) |
+-------+------+-------+-------+-------------------+------------------+
| 64 | 156 | 90130 | 90130 | 16849983073.488 | 16226452636.126 |
+-------+------+-------+-------+-------------------+------------------+
```

Run numbers/era are here: https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2016Analysis
