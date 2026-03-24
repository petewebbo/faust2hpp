import("stdfaust.lib");

declare name "ms_fader";
declare author "petewebbo";
declare license "GTFO"; 

process = 
    _,_ :
    lr2ms :
    (_*balance,_*(1-balance)) : 
    ms2lr : 
    _,_
with {
    balance = hslider("balance", 0.5, 0, 1, 0.001);
    lr2ms = _,_ <: ((_+_), (_-_)) : _,_;
    ms2lr = lr2ms : par(i,2,_*0.5);
};

