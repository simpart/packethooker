<?php
/**
 * @file   conf.php
 * @brief  config loader
 * @author simpart
 */
namespace pkh;
define ('DPKH_CONF'  , '/etc/pkthooker/pkthooker.cnf');
define ('DPKH_MSGKEY', '{@message}');

/*** global ***/
$gpkh_tgt_cnfkey = array(
    'HOOK_URI',
    'HOOK_PRM'
);


/*** function ***/
function getConf () {
    try {
        $ret_cnf = array();
        $cnf     = file(DPKH_CONF);
        if (false === $cnf) {
            throw new \Exception('failed load config');
        }
        
        $key_val = null;
        foreach ($cnf as $line) {
            $key_val = getKeyValue($line);
            if (null === $key_val) {
                continue;
            }
            $ret_cnf[$key_val[0]] = $key_val[1];
        }
        
        
        return $ret_cnf;
    } catch (\Exception $e) {
        throw new \Exception(
            PHP_EOL   .
            'File:'   . __FILE__   . ',' .
            'Line:'   . __line__   . ',' .
            'Func:' . __FUNCTION__ . ',' .
            $e->getMessage()
        );
    }
} 



function getKeyValue ($line) {
    try {
        global $gpkh_tgt_cnfkey;
        
        /* check exists contents */
        if (PHP_EOL == $line) {
            return null;
        }
        /* check comment */
        if ('#' == $line[0]) {
            return null;
        }
        
        /* check key name */
        $ex_line = explode('=', $line);
        if (2 > count($ex_line)) {
            return null;
        } else if (2 < count($ex_line)) {
            $rmk_val = '';
            foreach ($ex_line as $ex_key => $elm) {
                if (0 == $ex_key) {
                    continue;
                }
                if (0 !== strcmp('', $rmk_val)) {
                    $rmk_val .= '=';
                }
                $rmk_val .= $elm;
            }
            $ex_line = array($ex_line[0], $rmk_val);
        }
        
        foreach ($gpkh_tgt_cnfkey as $key) {
            if (false !== strpos($ex_line[0], $key)) {
                return array(
                    encode($ex_line[0]),    /* key */
                    encodeVal($ex_line[1])  /* value */
                );
            }
        }
        
        return null;
    } catch (\Exception $e) {
        throw new \Exception(
            PHP_EOL   .
            'File:'   . __FILE__   . ',' .
            'Line:'   . __line__   . ',' .
            'Func:' . __FUNCTION__ . ',' .
            $e->getMessage()
        );
    }
}

function encode ($cnf) {
    try {
        $tmp    = null;
        $ex_cnf = explode(' ', $cnf);
        if (1 < count($ex_cnf)) {
            foreach ($ex_cnf as $cnf_elm) {
                if (0 < strlen($cnf_elm)) {
                    $ex_cnf = array($cnf_elm);
                    break;
                }
            }
        }
        
        $ex_cnf = explode('"', $ex_cnf[0]);
        if (1 < count($ex_cnf)) {
            foreach ($ex_cnf as $cnf_elm) {
                if (0 < strlen($cnf_elm)) {
                    $ex_cnf = array($cnf_elm);
                    break;
                }
            }
        }
        return $ex_cnf[0];
    } catch (\Exception $e) {
        throw new \Exception(
            PHP_EOL   .
            'File:'   . __FILE__   . ',' .
            'Line:'   . __line__   . ',' .
            'Func:' . __FUNCTION__ . ',' .
            $e->getMessage()
        );
    }
}

function encodeVal ($val) {
    try {
        global $gpkh_msg;
        
        $tmp_val = encode($val);
        $tmp_val = explode(',', $tmp_val);
        if (1 === count($tmp_val)) {
            return $tmp_val[0];
        }
        
        $ret_val = array();
        foreach ($tmp_val as $val_elm) {
            $key_val = explode('=', $val_elm);
            if (2 !== count($key_val)) {
                throw new \Exception('invalid parameter');
            } else {
                if (0 === strcmp($key_val[1], '{@message}')) {
                    $key_val[1] = $gpkh_msg;
                }
                $ret_val[$key_val[0]] = $key_val[1];
            }
        }
        
        return $ret_val;
    } catch (\Exception $e) {
        throw new \Exception(
            PHP_EOL   .
            'File:'   . __FILE__   . ',' .
            'Line:'   . __line__   . ',' .
            'Func:' . __FUNCTION__ . ',' .
            $e->getMessage()
        );
    }
}
/* end of file */
