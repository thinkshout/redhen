<?php

$view = new view;
$view->name = 'redhen_contacts';
$view->description = '';
$view->tag = '';
$view->base_table = 'redhen_contact';
$view->human_name = 'test';
$view->core = 7;
$view->api_version = '3.0-alpha1';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Defaults */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'contact_id' => 'contact_id',
  'first_name' => 'first_name',
  'last_name' => 'last_name',
  'revision_id' => 'revision_id',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'contact_id' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
  ),
  'first_name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
  ),
  'last_name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
  ),
  'revision_id' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
  ),
);
$handler->display->display_options['style_options']['override'] = 1;
$handler->display->display_options['style_options']['sticky'] = 0;
/* Empty text: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = FALSE;
$handler->display->display_options['empty']['area']['content'] = 'No contacts available.';
/* Field: Contacts: Last name */
$handler->display->display_options['fields']['last_name']['id'] = 'last_name';
$handler->display->display_options['fields']['last_name']['table'] = 'redhen_contact';
$handler->display->display_options['fields']['last_name']['field'] = 'last_name';
$handler->display->display_options['fields']['last_name']['alter']['alter_text'] = 0;
$handler->display->display_options['fields']['last_name']['alter']['make_link'] = 0;
$handler->display->display_options['fields']['last_name']['alter']['absolute'] = 0;
$handler->display->display_options['fields']['last_name']['alter']['trim'] = 0;
$handler->display->display_options['fields']['last_name']['alter']['nl2br'] = 0;
$handler->display->display_options['fields']['last_name']['alter']['word_boundary'] = 1;
$handler->display->display_options['fields']['last_name']['alter']['ellipsis'] = 1;
$handler->display->display_options['fields']['last_name']['alter']['strip_tags'] = 0;
$handler->display->display_options['fields']['last_name']['alter']['html'] = 0;
$handler->display->display_options['fields']['last_name']['element_label_colon'] = 1;
$handler->display->display_options['fields']['last_name']['element_default_classes'] = 1;
$handler->display->display_options['fields']['last_name']['hide_empty'] = 0;
$handler->display->display_options['fields']['last_name']['empty_zero'] = 0;
/* Field: Contacts: First name */
$handler->display->display_options['fields']['first_name']['id'] = 'first_name';
$handler->display->display_options['fields']['first_name']['table'] = 'redhen_contact';
$handler->display->display_options['fields']['first_name']['field'] = 'first_name';
$handler->display->display_options['fields']['first_name']['alter']['alter_text'] = 0;
$handler->display->display_options['fields']['first_name']['alter']['make_link'] = 0;
$handler->display->display_options['fields']['first_name']['alter']['absolute'] = 0;
$handler->display->display_options['fields']['first_name']['alter']['trim'] = 0;
$handler->display->display_options['fields']['first_name']['alter']['nl2br'] = 0;
$handler->display->display_options['fields']['first_name']['alter']['word_boundary'] = 1;
$handler->display->display_options['fields']['first_name']['alter']['ellipsis'] = 1;
$handler->display->display_options['fields']['first_name']['alter']['strip_tags'] = 0;
$handler->display->display_options['fields']['first_name']['alter']['html'] = 0;
$handler->display->display_options['fields']['first_name']['element_label_colon'] = 1;
$handler->display->display_options['fields']['first_name']['element_default_classes'] = 1;
$handler->display->display_options['fields']['first_name']['hide_empty'] = 0;
$handler->display->display_options['fields']['first_name']['empty_zero'] = 0;
/* Field: Contacts: Link */
$handler->display->display_options['fields']['view_contact']['id'] = 'view_contact';
$handler->display->display_options['fields']['view_contact']['table'] = 'redhen_contact';
$handler->display->display_options['fields']['view_contact']['field'] = 'view_contact';
$handler->display->display_options['fields']['view_contact']['label'] = '';
$handler->display->display_options['fields']['view_contact']['alter']['alter_text'] = 0;
$handler->display->display_options['fields']['view_contact']['alter']['make_link'] = 0;
$handler->display->display_options['fields']['view_contact']['alter']['absolute'] = 0;
$handler->display->display_options['fields']['view_contact']['alter']['external'] = 0;
$handler->display->display_options['fields']['view_contact']['alter']['trim'] = 0;
$handler->display->display_options['fields']['view_contact']['alter']['nl2br'] = 0;
$handler->display->display_options['fields']['view_contact']['alter']['word_boundary'] = 1;
$handler->display->display_options['fields']['view_contact']['alter']['ellipsis'] = 1;
$handler->display->display_options['fields']['view_contact']['alter']['strip_tags'] = 0;
$handler->display->display_options['fields']['view_contact']['alter']['html'] = 0;
$handler->display->display_options['fields']['view_contact']['element_label_colon'] = 1;
$handler->display->display_options['fields']['view_contact']['element_default_classes'] = 1;
$handler->display->display_options['fields']['view_contact']['hide_empty'] = 0;
$handler->display->display_options['fields']['view_contact']['empty_zero'] = 0;
/* Field: Contacts: Edit link */
$handler->display->display_options['fields']['edit_contact']['id'] = 'edit_contact';
$handler->display->display_options['fields']['edit_contact']['table'] = 'redhen_contact';
$handler->display->display_options['fields']['edit_contact']['field'] = 'edit_contact';
$handler->display->display_options['fields']['edit_contact']['label'] = '';
$handler->display->display_options['fields']['edit_contact']['alter']['alter_text'] = 0;
$handler->display->display_options['fields']['edit_contact']['alter']['make_link'] = 0;
$handler->display->display_options['fields']['edit_contact']['alter']['absolute'] = 0;
$handler->display->display_options['fields']['edit_contact']['alter']['external'] = 0;
$handler->display->display_options['fields']['edit_contact']['alter']['trim'] = 0;
$handler->display->display_options['fields']['edit_contact']['alter']['nl2br'] = 0;
$handler->display->display_options['fields']['edit_contact']['alter']['word_boundary'] = 1;
$handler->display->display_options['fields']['edit_contact']['alter']['ellipsis'] = 1;
$handler->display->display_options['fields']['edit_contact']['alter']['strip_tags'] = 0;
$handler->display->display_options['fields']['edit_contact']['alter']['html'] = 0;
$handler->display->display_options['fields']['edit_contact']['element_label_colon'] = 1;
$handler->display->display_options['fields']['edit_contact']['element_default_classes'] = 1;
$handler->display->display_options['fields']['edit_contact']['hide_empty'] = 0;
$handler->display->display_options['fields']['edit_contact']['empty_zero'] = 0;
/* Field: Contacts: Delete link */
$handler->display->display_options['fields']['delete_contact']['id'] = 'delete_contact';
$handler->display->display_options['fields']['delete_contact']['table'] = 'redhen_contact';
$handler->display->display_options['fields']['delete_contact']['field'] = 'delete_contact';
$handler->display->display_options['fields']['delete_contact']['label'] = '';
$handler->display->display_options['fields']['delete_contact']['alter']['alter_text'] = 0;
$handler->display->display_options['fields']['delete_contact']['alter']['make_link'] = 0;
$handler->display->display_options['fields']['delete_contact']['alter']['absolute'] = 0;
$handler->display->display_options['fields']['delete_contact']['alter']['external'] = 0;
$handler->display->display_options['fields']['delete_contact']['alter']['trim'] = 0;
$handler->display->display_options['fields']['delete_contact']['alter']['nl2br'] = 0;
$handler->display->display_options['fields']['delete_contact']['alter']['word_boundary'] = 1;
$handler->display->display_options['fields']['delete_contact']['alter']['ellipsis'] = 1;
$handler->display->display_options['fields']['delete_contact']['alter']['strip_tags'] = 0;
$handler->display->display_options['fields']['delete_contact']['alter']['html'] = 0;
$handler->display->display_options['fields']['delete_contact']['element_label_colon'] = 1;
$handler->display->display_options['fields']['delete_contact']['element_default_classes'] = 1;
$handler->display->display_options['fields']['delete_contact']['hide_empty'] = 0;
$handler->display->display_options['fields']['delete_contact']['empty_zero'] = 0;
$translatables['redhen_contacts'] = array(
  t('Defaults'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort By'),
  t('Asc'),
  t('Desc'),
  t('Items per page'),
  t('- All -'),
  t('Offset'),
  t('No contacts available.'),
  t('Last name'),
  t('First name'),
);
