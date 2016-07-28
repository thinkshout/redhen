<?php

/**
 * @file
 * Contains note.page.inc.
 *
 * Page callback for RedHen Note entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for RedHen Note templates.
 *
 * Default template: note.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_note(array &$variables) {
  // Fetch Note Entity Object.
  $note = $variables['elements']['#note'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}
