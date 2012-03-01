<?php

/**
 * @file
 * 
 */

/**
 * Wrapper for system_admin_menu_block_page().
 */
function redhen_admin_menu_block_page() {
  module_load_include('inc', 'system', 'system.admin');
  return system_admin_menu_block_page();
}
