<?php

namespace Drupal\redhen_contact\Controller;

use Drupal\Component\Utility\Xss;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Entity\Element\EntityAutocomplete;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpFoundation\Request;

/**
 * Class ContactAutocompleteController.
 */
class ContactAutocompleteController extends ControllerBase {

  /**
   * The current primary database.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected $database;

  /**
   * {@inheritDoc}
   */
  public static function create(ContainerInterface $container) {
    $instance = new static();
    $instance->database = $container->get('database');
    return $instance;
  }

  /**
   * Handler for autocomplete request.
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The request.
   *
   * @return \Symfony\Component\HttpFoundation\JsonResponse
   *   The response.
   */
  public function autocomplete(Request $request) {
    $results = [];

    // Sanitize input parameters.
    $q = trim(str_replace('%', '', Xss::filter($request->query->get('q'))));
    $match_operator = Xss::filter($request->query->get('match_operator'));
    $match_limit = intval($request->query->get('match_limit'));
    if ($match_limit <= 0) {
      $match_limit = 10;
    }
    elseif ($match_limit > 100) {
      $match_limit = 100;
    }

    if ($q) {
      if ($match_operator == 'STARTS_WITH') {
        $full_name_like = preg_replace('/\s+/', '% ', $q) . '%';
        $email_like = $q . '%';
      }
      else {
        $full_name_like = '%' . preg_replace('/\s+/', '% %', $q) . '%';
        $email_like = '%' . $q . '%';
      }

      $query = $this->database->query('
        SELECT id
        FROM redhen_contact
        WHERE CONCAT(first_name, \' \', last_name) LIKE :full_name_like
          OR email LIKE :email_like
        ORDER BY first_name ASC, last_name ASC
        LIMIT ' . $match_limit . '
      ', [
        ':full_name_like' => $full_name_like,
        ':email_like' => $email_like,
      ]);
      $contact_ids = $query->fetchCol();

      $contacts = $this->entityTypeManager()->getStorage('redhen_contact')->loadMultiple($contact_ids);
      /** @var \Drupal\redhen_contact\Entity\Contact $contact */
      foreach ($contacts as $contact) {
        $results[] = [
          'value' => EntityAutocomplete::getEntityLabels([$contact]),
          'label' => $contact->label() . ' (' . $contact->getEmail() . ')',
        ];
      }
    }

    return new JsonResponse($results);
  }

}
