<?php

/**
 * @file
 * Contains redhen_dedupe.theme.inc.
 */

use Drupal\Core\Render\Element;

/**
 * Themes the field associations on a fieldmap edit form into a table.
 */
function template_preprocess_redhen_dedupe_form_table(&$variables) {
  $elements = $variables['elements'];
  // Build the rows array.
  $rows = [];
  foreach (Element::children($elements) as $item_key) {
    $item = &$elements[$item_key];
    $data = [$item['#title']];
    foreach (Element::children($item) as $element) {
      if (isset($item['#options']) && $item['#options'][$element] === REDHEN_DEDUPE_NOT_APPLICABLE) {
        $cell_data = t('Field not applicable to this bundle');
      }
      else {
        $cell_data = \Drupal::service("renderer")->render($item[$element]);
      }

      $cell = ['data' => $cell_data];
      if (isset($item[$element]['#attributes'])) {
        foreach ($item[$element]['#attributes'] as $key => $value) {
          $cell[$key] = $value;
        }
      }
      $data[] = $cell;

    }
    $row = ['data' => $data];
    if (isset($item_key['#attributes'])) {
      foreach ($item_key['#attributes'] as $key => $value) {
        $row[$key] = $value;
      }
    }
    $rows[] = $row;
  }

  $config = [
    'rows' => $rows,
  ];
  if (isset($elements['#header'])) {
    $config['header'] = $elements['#header'];
  }
  if (isset($elements['#attributes']) && is_array($elements['#attributes'])) {
    $config['attributes'] = $elements['#attributes'];
  }

  $table = [
    '#type' => 'table',
    '#header' => $config['header'],
    '#attributes' => $config['attributes'],
    '#rows' => $config['rows'],
  ];

  $variables['content'] = $table;
}
