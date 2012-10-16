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
  module_load_include('inc', 'redhen_note', 'includes/redhen_note.forms');
  return drupal_get_form('redhen_note_form', $redhen_note, $entity);
}

/*
 * Show notes and actions
 */
function redhen_note_notes_page($entity, $entity_type) {
  list($entity_id, ,) = entity_extract_ids($entity_type, $entity);

  $header = array(
    t('Body'),
    t('Note Type'),
    t('Author'),
    'updated' => array(
      'field' => 'updated',
      'type' => 'property',
      'data' => t('Updated'),
      'sort' => 'asc',
      'specifier' => 'updated'
    ),
    'created' => array(
      'field' => 'created',
      'type' => 'property',
      'data' => t('Created'),
      'sort' => 'asc',
      'specifier' => 'created'
    ),
    t('Actions'),
  );

  // need to ensure the query doesn't execute when posing the form
  $result = FALSE;
  if (!isset($_POST['form_id'])) {
    $bundle = '';
    $properties = isset($_GET['properties']) ? $_GET['properties'] : array();
    $properties['entity_id'] = $entity_id;
    $properties['entity_type'] = $entity_type;
    $fields = isset($_GET['fields']) ? $_GET['fields'] : array();
    $result = redhen_filter_query('redhen_note', $header, $bundle, $properties, $fields);
  }

  $rows = array();
  if ($result) {
    $notes = redhen_note_load_multiple(array_keys($result['redhen_note']));
    foreach ($notes as $note) {
      $wrapper = entity_metadata_wrapper('redhen_note', $note);
      $author = $wrapper->author->value();
      $entity = $wrapper->entity->value();
      $entity_uri = entity_uri($note->entity_type, $entity);

      $actions = array(
        l(t('view'), $entity_uri['path'] . '/note/' . $note->note_id . '/view'),
        l(t('edit'), $entity_uri['path'] .  '/note/' . $note->note_id . '/view/edit', array('query' => drupal_get_destination())),
        l(t('delete'), $entity_uri['path'] .  '/note/' . $note->note_id . '/view/delete', array('query' => drupal_get_destination())),
      );

      $teaser = field_view_value('redhen_note', $note, 'redhen_note_body',
        $wrapper->redhen_note_body->value(),
        array(
          'label' => 'hidden',
          'type' => 'text_trimmed',
          'settings' => array('trim_length' => 100)
      ));

      $rows[] = array(
        'data' => array(
          render($teaser),
          $wrapper->redhen_note_type->value()->name,
          (isset($author->name)) ? $author->name : '',
          redhen_format_date($note->updated),
          redhen_format_date($note->created),
          implode(' | ', $actions)
        )
      );
    }
  }
  else {
    // no results, set a message
    $rows[] = array(
      'data' => array(
        array(
          'data' => t('Sorry, there are no notes that match your criteria.'),
          'colspan' => 7
        )
      )
    );
  }

  $render = array(
    'form' => drupal_get_form('redhen_filter_form', 'redhen_note'),
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
