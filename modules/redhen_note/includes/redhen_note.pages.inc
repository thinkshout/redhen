<?php

/**
 * @file
 * Page callbacks for redhen ntoes.
 */

module_load_include('inc', 'redhen', 'includes/redhen.forms');

/*
 * Create new Redhen note object and call note form
 */
function redhen_note_add($entity, $entity_type) {
  $redhen_note = entity_get_controller('redhen_note')->create(array('type' => 'redhen_note'));
  $redhen_note->entity_type = $entity_type;
  $redhen_note->entity = $entity;
  module_load_include('inc', 'redhen_note', 'includes/redhen_note.forms');
  return drupal_get_form('redhen_note_form', $redhen_note);
}

/*
 * Show notes and actions
 */
function redhen_note_notes_page($entity, $entity_type) {

  list($entity_id,,) = entity_extract_ids($entity_type, $entity);

  switch ($entity_type) {
    case 'redhen_contact':
      $path = 'contact';
      break;
    case 'redhen_org':
      $path = 'org';
      break;
  }

  $header = array(
    'note_id' => array('field' => 'note_id', 'type' => 'property', 'data' => 'Note ID', 'sort' => 'asc', 'specifier' => 'note_id'),
    'Body',
    'Type',
    'Author',
    'updated' => array('field' => 'updated', 'type' => 'property', 'data' => 'Updated', 'sort' => 'asc', 'specifier' => 'updated'),
    'Actions',
  );

  // need to ensure the query doesn't execute when posing the form
  $result = FALSE;
  if (!isset($_POST['form_id'])) {
    $bundle = '';
    $properties = array('entity_type' => $entity_type, 'entity_id' => $entity_id);
    $fields = isset($_GET['fields']) ? $_GET['fields'] : array();
    $result = redhen_filter_query('redhen_note', $header, $bundle, $properties, $fields);
  }


  $rows = array();
  if ($result) {
    $notes = redhen_note_load_multiple(array_keys($result['redhen_note']));
    foreach($notes as $note) {
      $actions = array(
        l('view', 'redhen/' . $path . '/' . $entity_id . '/note/' . $note->note_id . '/view'),
        l('edit', 'redhen/' . $path . '/' . $entity_id . '/note/' . $note->note_id . '/view/edit', array('query' => drupal_get_destination())),
        l('delete', 'redhen/' . $path . '/' . $entity_id . '/note/' . $note->note_id . '/view/delete', array('query' => drupal_get_destination())),
      );
      //dpm($note);

      $body = $note->redhen_note_body[LANGUAGE_NONE][0]['value'];
      $type_tid = $note->redhen_note_type[LANGUAGE_NONE][0]['tid'];
      $type = taxonomy_term_load($type_tid);
      //dpm($category);
      $rows[] = array(
        'data' => array(
          $note->note_id,
          substr($body,0,5) . ' ...',
          (isset($type->name)) ? $type->name: '',
          $note->author->name,
          format_date($note->updated),
          implode(' | ', $actions))
      );
    }
  }
  else {
    // no results, set a message
    $rows[] = array(
      'data' => array(
        array(
          'data' => t('Sorry, there are no note that match your criteria.'), 'colspan' => 5))
    );
  }

  $render = array(
//    /'form' => drupal_get_form('redhen_filter_form', 'redhen_note'),
    'table' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows
    ),
    'pager' => array(
    '#theme' => 'pager'
    )
  );

  return $render;
}