#/bin/bash

Rscript param_opt.R Plot5/T 8 > Plot5/optimal.Rlog 2>&1 &
Rscript param_opt.R Plot5/T 64 > Plot5/proportional.Rlog 2>&1 &
Rscript param_opt.R Plot5/T 16 > Plot5/greedy.Rlog 2>&1 &

Rscript param_opt.R Plot4/T 6 > Plot4/optimal.Rlog 2>&1 &
Rscript param_opt.R Plot4/T 16 > Plot4/proportional.Rlog 2>&1 &
Rscript param_opt.R Plot4/T 16 > Plot4/greedy.Rlog 2>&1 &

Rscript param_opt.R Plot3/T 2 > Plot3/optimal.Rlog 2>&1 &
Rscript param_opt.R Plot3/T 4 > Plot3/proportional.Rlog 2>&1 &
Rscript param_opt.R Plot3/T 2 > Plot3/greedy.Rlog 2>&1 &

Rscript param_opt.R Plot2/T 8 > Plot2/optimal.Rlog 2>&1 &
Rscript param_opt.R Plot2/T 9 > Plot2/proportional.Rlog 2>&1 &
Rscript param_opt.R Plot2/T 12 > Plot2/greedy.Rlog 2>&1 &

Rscript param_opt.R Plot1/T 2 > Plot1/optimal.Rlog 2>&1 &
Rscript param_opt.R Plot1/T 4 > Plot1/proportional.Rlog 2>&1 &
Rscript param_opt.R Plot1/T 4 > Plot1/greedy.Rlog 2>&1 &


